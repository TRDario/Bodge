#include "../../include/engine.hpp"
#include "../../include/state/replay_state.hpp"
#include "../../include/state/replays_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The number of replays per page.
constexpr size_t REPLAYS_PER_PAGE{5};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

replays_state::replays_state()
	: _substate{substate::RETURNING_FROM_REPLAY}
	, _page{0}
	, _timer{0}
	, _game{make_unique<game>(MENU_GAMEMODES[rand(rng, MENU_GAMEMODES.size())], rand<std::uint64_t>(rng))}
	, _replays{load_replay_headers()}
{
	set_up_ui();
}

replays_state::replays_state(unique_ptr<game>&& game)
	: _substate{substate::IN_REPLAYS}, _page{0}, _timer{0}, _game{std::move(game)}, _replays{load_replay_headers()}
{
	set_up_ui();
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

u32 replays_state::type() const noexcept
{
	return ID;
}

unique_ptr<state> replays_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

unique_ptr<state> replays_state::update(tr::duration)
{
	++_timer;
	_game->update({});
	_ui.update();

	switch (_substate) {
	case substate::RETURNING_FROM_REPLAY:
		if (_timer >= 0.5_s) {
			_timer = 0;
			_substate = substate::IN_REPLAYS;
		}
		return nullptr;
	case substate::IN_REPLAYS:
		return nullptr;
	case substate::SWITCHING_PAGE:
		if (_timer >= 0.5_s) {
			_timer = 0;
			_substate = substate::IN_REPLAYS;
		}
		else if (_timer == 0.25_s) {
			map<string, replay_header>::iterator it{std::next(_replays.begin(), REPLAYS_PER_PAGE * _page)};
			for (size_t i = 0; i < REPLAYS_PER_PAGE; ++i) {
				replay_widget& widget{_ui.get<replay_widget>(format("replay{}", i))};
				widget.it = it != _replays.end() ? optional{it++} : std::nullopt;
				widget.pos = {i % 2 == 0 ? 750 : 250, vec2{widget.pos}.y};
				widget.pos.change({500, vec2{widget.pos}.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
		return nullptr;
	case substate::STARTING_REPLAY:
		return _timer >= 0.5_s
				   ? make_unique<replay_state>(make_unique<replay_game>(_selected->second.gamemode, replay{_selected->first}), true)
				   : nullptr;
	case substate::ENTERING_TITLE:
		return _timer >= 0.5_s ? make_unique<title_state>(std::move(_game)) : nullptr;
	}
}

void replays_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float replays_state::fade_overlay_opacity() const noexcept
{
	switch (_substate) {
	case substate::RETURNING_FROM_REPLAY:
		return 1 - _timer / 0.5_sf;
	case substate::IN_REPLAYS:
	case substate::SWITCHING_PAGE:
	case substate::ENTERING_TITLE:
		return 0;
	case substate::STARTING_REPLAY:
		return _timer / 0.5_sf;
	}
}

void replays_state::set_up_ui()
{
	widget& title{_ui.emplace<text_widget>("replays", vec2{500, -50}, TOP_CENTER, font::LANGUAGE, ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	const status_callback STATUS_CB{[this] { return _substate == substate::IN_REPLAYS; }};
	const auto REPLAY_ACTION_CB{[this](map<string, replay_header>::iterator it) {
		_substate = substate::STARTING_REPLAY;
		_timer = 0;
		_selected = it;
		set_up_exit_animation();
	}};

	action_callback exit_action_cb{[this] {
		_substate = substate::ENTERING_TITLE;
		_timer = 0;
		set_up_exit_animation();
	}};
	widget& exit{_ui.emplace<clickable_text_widget>("exit", vec2{500, 1050}, BOTTOM_CENTER, font::LANGUAGE, 48, DEFAULT_TEXT_CALLBACK,
													STATUS_CB, exit_action_cb, NO_TOOLTIP,
													vector<key_chord>{{key::ESCAPE}, {key::Q}, {key::E}})};
	exit.pos.change({500, 1000}, 0.5_s);
	exit.unhide(0.5_s);

	if (_replays.empty()) {
		widget& no_replays_found{_ui.emplace<text_widget>("no_replays_found", vec2{600, 467}, TOP_CENTER, font::LANGUAGE, ttf_style::NORMAL,
														  64, DEFAULT_TEXT_CALLBACK, rgba8{128, 128, 128, 128})};
		no_replays_found.pos.change({500, 467}, 0.5_s);
		no_replays_found.unhide(0.5_s);
		return;
	}

	map<string, replay_header>::iterator it{_replays.begin()};
	for (size_t i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const optional<map<string, replay_header>::iterator> opt_it{it != _replays.end() ? optional{it++} : std::nullopt};
		widget& widget{_ui.emplace<replay_widget>(format("replay{}", i), vec2{i % 2 == 0 ? 250 : 750, 179 + 150 * i}, CENTER, STATUS_CB,
												  REPLAY_ACTION_CB, opt_it, tr::make_top_row_keycode(i + 1))};
		widget.pos.change({500, 179 + 150 * i}, 0.5_s);
		widget.unhide(0.5_s);
	}

	const text_callback current_page_text_cb{
		[this](const static_string<30>&) { return format("{}/{}", _page + 1, max(_replays.size() - 1, 0uz) / REPLAYS_PER_PAGE + 1); }};
	widget& current_page{_ui.emplace<text_widget>("current_page", vec2{500, 1050}, BOTTOM_CENTER, font::LANGUAGE, ttf_style::NORMAL, 48,
												  current_page_text_cb)};
	current_page.pos.change({500, 950}, 0.5_s);
	current_page.unhide(0.5_s);

	const status_callback page_dec_status_cb{[this] { return _substate == substate::IN_REPLAYS && _page > 0; }};
	const action_callback page_dec_action_cb{[this] {
		_substate = substate::SWITCHING_PAGE;
		_timer = 0;
		--_page;
		set_up_page_switch_animation();
	}};
	widget& page_dec{_ui.emplace<arrow_widget>("page_dec", vec2{-50, 942.5}, BOTTOM_LEFT, false, page_dec_status_cb, page_dec_action_cb,
											   vector<key_chord>{{key::LEFT}})};
	page_dec.pos.change({10, 942.5}, 0.5_s);
	page_dec.unhide(0.5_s);

	const status_callback page_inc_status_cb{
		[this] { return _substate == substate::IN_REPLAYS && _page < (max(_replays.size() - 1, 0uz) / REPLAYS_PER_PAGE); }};
	const action_callback page_inc_action_cb{[this] {
		_substate = substate::SWITCHING_PAGE;
		_timer = 0;
		++_page;
		set_up_page_switch_animation();
	}};
	widget& page_inc{_ui.emplace<arrow_widget>("page_inc", vec2{1050, 942.5}, BOTTOM_RIGHT, true, page_inc_status_cb, page_inc_action_cb,
											   vector<key_chord>{{key::RIGHT}})};
	page_inc.pos.change({990, 942.5}, 0.5_s);
	page_inc.unhide(0.5_s);
}

void replays_state::set_up_page_switch_animation() noexcept
{
	for (size_t i = 0; i < REPLAYS_PER_PAGE; i++) {
		widget& widget{_ui.get(format("replay{}", i))};
		widget.pos.change({i % 2 == 0 ? 750 : 250, vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
}

void replays_state::set_up_exit_animation() noexcept
{
	_ui.get("replays").pos.change({500, -50}, 0.5_s);
	_ui.get("exit").pos.change({500, 1050}, 0.5_s);
	if (_replays.empty()) {
		_ui.get("no_replays_found").pos.change({400, 467}, 0.5_s);
	}
	else {
		for (size_t i = 0; i < REPLAYS_PER_PAGE; i++) {
			widget& widget{_ui.get(format("replay{}", i))};
			widget.pos.change({i % 2 == 0 ? 750 : 250, vec2{widget.pos}.y}, 0.5_s);
		}
		_ui.get("current_page").pos.change({500, 1050}, 0.5_s);
		_ui.get("page_dec").pos.change({-50, 942.5}, 0.5_s);
		_ui.get("page_inc").pos.change({1050, 942.5}, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}