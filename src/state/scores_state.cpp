#include "../../include/engine.hpp"
#include "../../include/state/scores_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The number of scores per page.
constexpr size_t SCORES_PER_PAGE{8};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

scores_state::scores_state(unique_ptr<game>&& game)
	: _substate{substate::IN_SCORES}, _page{0}, _timer{0}, _game{std::move(game)}, _current{scorefile.scores.begin()}
{
	widget& title{_ui.emplace<text_widget>("scores", vec2{500, -50}, TOP_CENTER, font::LANGUAGE, ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	const status_callback STATUS_CB{[this] { return _substate == substate::IN_SCORES; }};

	action_callback exit_action_cb{[this] {
		_substate = substate::EXITING_TO_TITLE;
		_timer = 0;
		set_up_exit_animation();
	}};
	widget& exit{_ui.emplace<clickable_text_widget>("exit", vec2{500, 1050}, BOTTOM_CENTER, font::LANGUAGE, 48, DEFAULT_TEXT_CALLBACK,
													STATUS_CB, exit_action_cb, NO_TOOLTIP,
													vector<key_chord>{{key::ESCAPE}, {key::Q}, {key::E}})};
	exit.pos.change({500, 1000}, 0.5_s);
	exit.unhide(0.5_s);

	if (scorefile.scores.empty()) {
		widget& no_scores_found{_ui.emplace<text_widget>("no_scores_found", vec2{600, 467}, TOP_CENTER, font::LANGUAGE, ttf_style::NORMAL,
														 64, DEFAULT_TEXT_CALLBACK, rgba8{128, 128, 128, 128})};
		no_scores_found.pos.change({500, 467}, 0.5_s);
		no_scores_found.unhide(0.5_s);
		return;
	}

	for (size_t i = 0; i < SCORES_PER_PAGE; ++i) {
		widget& widget{_ui.emplace<score_widget>(format("score{}", i), vec2{i % 2 == 0 ? 250 : 750, 147 + 86 * i}, CENTER,
												 _page * SCORES_PER_PAGE + i + 1,
												 _current->second.size() > i ? std::to_address(_current->second.begin() + i) : nullptr)};
		widget.pos.change({500, 147 + 86 * i}, 0.5_s);
		widget.unhide(0.5_s);
	}

	const tooltip_callback current_gamemode_tooltip_cb{[this] {
		return _current->first.builtin ? string{localization[_current->first.description]} : string{_current->first.description};
	}};
	const text_callback current_gamemode_text_cb{[this](const static_string<30>&) {
		return _current->first.builtin ? string{localization[_current->first.name]} : string{_current->first.name};
	}};
	widget& current_gamemode{_ui.emplace<text_widget>("current_gamemode", vec2{500, 1050}, BOTTOM_CENTER, true, current_gamemode_tooltip_cb,
													  false, vector<key_chord>{}, font::LANGUAGE, ttf_style::NORMAL, halign::CENTER, 48,
													  UNLIMITED_WIDTH, rgba8{160, 160, 160, 160}, current_gamemode_text_cb)};
	current_gamemode.pos.change({500, 900}, 0.5_s);
	current_gamemode.unhide(0.5_s);

	const status_callback gamemode_change_status_cb{[this] { return _substate == substate::IN_SCORES && scorefile.scores.size() != 1; }};
	const action_callback gamemode_dec_action_cb{[this] {
		_substate = substate::SWITCHING_PAGE;
		_timer = 0;
		_page = 0;
		if (_current == scorefile.scores.begin()) {
			_current = scorefile.scores.end();
		}
		--_current;
		set_up_page_switch_animation();
	}};
	widget& gamemode_dec{_ui.emplace<arrow_widget>("gamemode_dec", vec2{-50, 892.5}, BOTTOM_LEFT, false, gamemode_change_status_cb,
												   gamemode_dec_action_cb, vector<key_chord>{{key::LEFT, mods::SHIFT}})};
	gamemode_dec.pos.change({10, 892.5}, 0.5_s);
	gamemode_dec.unhide(0.5_s);

	const action_callback gamemode_inc_action_cb{[this] {
		_substate = substate::SWITCHING_PAGE;
		_timer = 0;
		_page = 0;
		if (++_current == scorefile.scores.end()) {
			_current = scorefile.scores.begin();
		}
		set_up_page_switch_animation();
	}};
	widget& gamemode_inc{_ui.emplace<arrow_widget>("gamemode_inc", vec2{1050, 892.5}, BOTTOM_RIGHT, true, gamemode_change_status_cb,
												   gamemode_inc_action_cb, vector<key_chord>{{key::RIGHT, mods::SHIFT}})};
	gamemode_inc.pos.change({990, 892.5}, 0.5_s);
	gamemode_inc.unhide(0.5_s);

	const text_callback current_page_text_cb{[this](const static_string<30>&) {
		return format("{}/{}", _page + 1, max(_current->second.size() - 1, 0uz) / SCORES_PER_PAGE + 1);
	}};
	widget& current_page{_ui.emplace<text_widget>("current_page", vec2{500, 1050}, BOTTOM_CENTER, font::LANGUAGE, ttf_style::NORMAL, 48,
												  current_page_text_cb)};
	current_page.pos.change({500, 950}, 0.5_s);
	current_page.unhide(0.5_s);

	const status_callback page_dec_status_cb{[this] { return _substate == substate::IN_SCORES && _page > 0; }};
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
		[this] { return _substate == substate::IN_SCORES && _page < (max(_current->second.size() - 1, 0uz) / SCORES_PER_PAGE); }};
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

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

u32 scores_state::type() const noexcept
{
	return ID;
}

unique_ptr<state> scores_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

unique_ptr<state> scores_state::update(tr::duration)
{
	++_timer;
	_game->update({});
	_ui.update();

	switch (_substate) {
	case substate::IN_SCORES:
		return nullptr;
	case substate::SWITCHING_PAGE:
		if (_timer >= 0.5_s) {
			_timer = 0;
			_substate = substate::IN_SCORES;
		}
		else if (_timer == 0.25_s) {
			for (size_t i = 0; i < SCORES_PER_PAGE; ++i) {
				score_widget& widget{_ui.get<score_widget>(format("score{}", i))};
				widget.rank = _page * SCORES_PER_PAGE + i + 1;
				widget.score = _current->second.size() > _page * SCORES_PER_PAGE + i
								   ? std::to_address(_current->second.begin() + _page * SCORES_PER_PAGE + i)
								   : nullptr;
				widget.pos = {i % 2 == 0 ? 750 : 250, vec2{widget.pos}.y};
				widget.pos.change({500, vec2{widget.pos}.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
		return nullptr;
	case substate::EXITING_TO_TITLE:
		return _timer >= 0.5_s ? make_unique<title_state>(std::move(_game)) : nullptr;
	}
}

void scores_state::draw()
{
	_game->add_to_renderer();
	engine::layered_renderer().add_color_quad(layer::GAME_OVERLAY, MENU_GAME_OVERLAY_QUAD);
	_ui.add_to_renderer();

	engine::layered_renderer().draw_up_to_layer(layer::UI, engine::screen());
	engine::batched_renderer().draw(engine::screen());
	engine::layered_renderer().draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void scores_state::set_up_page_switch_animation() noexcept
{
	for (size_t i = 0; i < SCORES_PER_PAGE; i++) {
		widget& widget{_ui.get(format("score{}", i))};
		widget.pos.change({i % 2 == 0 ? 750 : 250, vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
}

void scores_state::set_up_exit_animation() noexcept
{
	_ui.get("scores").pos.change({500, -50}, 0.5_s);
	_ui.get("exit").pos.change({500, 1050}, 0.5_s);
	if (scorefile.scores.empty()) {
		_ui.get("no_scores_found").pos.change({400, 467}, 0.5_s);
	}
	else {
		for (size_t i = 0; i < SCORES_PER_PAGE; i++) {
			widget& widget{_ui.get(format("score{}", i))};
			widget.pos.change({i % 2 == 0 ? 750 : 250, vec2{widget.pos}.y}, 0.5_s);
		}
		_ui.get("current_gamemode").pos.change({500, 1050}, 0.5_s);
		_ui.get("gamemode_dec").pos.change({-50, 892.5}, 0.5_s);
		_ui.get("gamemode_inc").pos.change({1050, 892.5}, 0.5_s);
		_ui.get("current_page").pos.change({500, 1050}, 0.5_s);
		_ui.get("page_dec").pos.change({-50, 942.5}, 0.5_s);
		_ui.get("page_inc").pos.change({1050, 942.5}, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}