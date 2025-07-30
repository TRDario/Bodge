#include "../../include/state/replays_state.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The number of replays per page.
constexpr std::size_t REPLAYS_PER_PAGE{5};

// Shortcuts of the exit button.
constexpr std::initializer_list<tr::key_chord> EXIT_SHORTCUTS{{tr::keycode::ESCAPE}, {tr::keycode::Q}, {tr::keycode::E}};
// Shortcuts of the page decrement button.
constexpr std::initializer_list<tr::key_chord> PAGE_DEC_SHORTCUTS{{tr::keycode::LEFT}};
// Shortcuts of the page increment button.
constexpr std::initializer_list<tr::key_chord> PAGE_INC_SHORTCUTS{{tr::keycode::RIGHT}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

replays_state::replays_state()
	: _substate{substate::RETURNING_FROM_REPLAY}
	, _page{0}
	, _timer{0}
	, _game{std::make_unique<game>(pick_menu_gamemode(), rng.generate<std::uint64_t>())}
	, _replays{load_replay_headers()}
{
	set_up_ui();
	audio::play_song(song::MENU, SKIP_MENU_SONG_INTRO, 0.5s);
}

replays_state::replays_state(std::unique_ptr<game>&& game)
	: _substate{substate::IN_REPLAYS}, _page{0}, _timer{0}, _game{std::move(game)}, _replays{load_replay_headers()}
{
	set_up_ui();
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> replays_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> replays_state::update(tr::duration)
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
			std::map<std::string, replay_header>::iterator it{std::next(_replays.begin(), REPLAYS_PER_PAGE * _page)};
			for (std::size_t i = 0; i < REPLAYS_PER_PAGE; ++i) {
				replay_widget& widget{_ui.get<replay_widget>(std::format("replay{}", i))};
				widget.it = it != _replays.end() ? std::optional{it++} : std::nullopt;
				widget.pos = {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y};
				widget.pos.change({500, glm::vec2{widget.pos}.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
		return nullptr;
	case substate::STARTING_REPLAY:
		return _timer >= 0.5_s
				   ? std::make_unique<game_state>(std::make_unique<replay_game>(_selected->second.gamemode, replay{_selected->first}),
												  game_type::REPLAY, true)
				   : nullptr;
	case substate::ENTERING_TITLE:
		return _timer >= 0.5_s ? std::make_unique<title_state>(std::move(_game)) : nullptr;
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
	// STATUS CALLBACKS

	const status_callback status_cb{[this] { return _substate == substate::IN_REPLAYS; }};
	const status_callback page_dec_status_cb{[this] { return _substate == substate::IN_REPLAYS && _page > 0; }};
	const status_callback page_inc_status_cb{
		[this] { return _substate == substate::IN_REPLAYS && _page < (std::max(_replays.size() - 1, std::size_t{0}) / REPLAYS_PER_PAGE); }};

	// ACTION CALLBACKS

	const auto replay_action_cb{[this](std::map<std::string, replay_header>::iterator it) {
		_substate = substate::STARTING_REPLAY;
		_timer = 0;
		_selected = it;
		set_up_exit_animation();
		audio::fade_song_out(0.5s);
	}};
	action_callback exit_action_cb{[this] {
		_substate = substate::ENTERING_TITLE;
		_timer = 0;
		set_up_exit_animation();
	}};
	const action_callback page_dec_action_cb{[this] {
		_substate = substate::SWITCHING_PAGE;
		_timer = 0;
		--_page;
		set_up_page_switch_animation();
	}};
	const action_callback page_inc_action_cb{[this] {
		_substate = substate::SWITCHING_PAGE;
		_timer = 0;
		++_page;
		set_up_page_switch_animation();
	}};

	//

	widget& title{_ui.emplace<text_widget>("replays", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	widget& exit{_ui.emplace<clickable_text_widget>("exit", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													DEFAULT_TEXT_CALLBACK, status_cb, exit_action_cb, NO_TOOLTIP, EXIT_SHORTCUTS,
													sound::CANCEL)};
	exit.pos.change({500, 1000}, 0.5_s);
	exit.unhide(0.5_s);

	if (_replays.empty()) {
		widget& no_replays_found{_ui.emplace<text_widget>("no_replays_found", glm::vec2{600, 467}, tr::align::TOP_CENTER, font::LANGUAGE,
														  tr::ttf_style::NORMAL, 64, DEFAULT_TEXT_CALLBACK, "80808080"_rgba8)};
		no_replays_found.pos.change({500, 467}, 0.5_s);
		no_replays_found.unhide(0.5_s);
		return;
	}

	std::map<std::string, replay_header>::iterator it{_replays.begin()};
	for (std::size_t i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const std::optional<std::map<std::string, replay_header>::iterator> opt_it{it != _replays.end() ? std::optional{it++}
																										: std::nullopt};
		const glm::vec2 pos{i % 2 == 0 ? 400 : 600, 183 + 150 * i};
		widget& widget{_ui.emplace<replay_widget>(std::format("replay{}", i), pos, tr::align::CENTER, status_cb, replay_action_cb, opt_it,
												  tr::make_top_row_keycode(i + 1))};
		widget.pos.change({500, 183 + 150 * i}, 0.5_s);
		widget.unhide(0.5_s);
	}

	const text_callback cur_page_text_cb{
		[this](auto&) { return std::format("{}/{}", _page + 1, std::max(_replays.size() - 1, std::size_t{0}) / REPLAYS_PER_PAGE + 1); }};
	widget& cur_page{_ui.emplace<text_widget>("cur_page", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL,
											  48, cur_page_text_cb)};
	cur_page.pos.change({500, 950}, 0.5_s);
	cur_page.unhide(0.5_s);

	widget& page_dec{_ui.emplace<arrow_widget>("page_dec", glm::vec2{-50, 942.5}, tr::align::BOTTOM_LEFT, false, page_dec_status_cb,
											   page_dec_action_cb, PAGE_DEC_SHORTCUTS)};
	page_dec.pos.change({10, 942.5}, 0.5_s);
	page_dec.unhide(0.5_s);

	widget& page_inc{_ui.emplace<arrow_widget>("page_inc", glm::vec2{1050, 942.5}, tr::align::BOTTOM_RIGHT, true, page_inc_status_cb,
											   page_inc_action_cb, PAGE_INC_SHORTCUTS)};
	page_inc.pos.change({990, 942.5}, 0.5_s);
	page_inc.unhide(0.5_s);
}

void replays_state::set_up_page_switch_animation() noexcept
{
	for (std::size_t i = 0; i < REPLAYS_PER_PAGE; i++) {
		widget& widget{_ui.get(std::format("replay{}", i))};
		widget.pos.change({i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
}

void replays_state::set_up_exit_animation() noexcept
{
	_ui.get("replays").pos.change(TOP_START_POS, 0.5_s);
	_ui.get("exit").pos.change(BOTTOM_START_POS, 0.5_s);
	if (_replays.empty()) {
		_ui.get("no_replays_found").pos.change({400, 467}, 0.5_s);
	}
	else {
		for (std::size_t i = 0; i < REPLAYS_PER_PAGE; i++) {
			widget& widget{_ui.get(std::format("replay{}", i))};
			widget.pos.change({i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.5_s);
		}
		_ui.get("cur_page").pos.change(BOTTOM_START_POS, 0.5_s);
		_ui.get("page_dec").pos.change({-50, 942.5}, 0.5_s);
		_ui.get("page_inc").pos.change({1050, 942.5}, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}