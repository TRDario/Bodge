#include "../../include/engine.hpp"
#include "../../include/state/scoreboards_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The number of scores per page.
constexpr std::size_t SCORES_PER_PAGE{8};
// Shortcuts of the exit button.
constexpr std::initializer_list<key_chord> EXIT_SHORTCUTS{{tr::keycode::ESCAPE}, {tr::keycode::Q}, {tr::keycode::E}};
// Shortcuts of the gamemode decrement button.
constexpr std::initializer_list<key_chord> GAMEMODE_DEC_SHORTCUTS{{tr::keycode::LEFT, tr::keymods::SHIFT}};
// Shortcuts of the gamemode increment button.
constexpr std::initializer_list<key_chord> GAMEMODE_INC_SHORTCUTS{{tr::keycode::RIGHT, tr::keymods::SHIFT}};
// Shortcuts of the page decrement button.
constexpr std::initializer_list<key_chord> PAGE_DEC_SHORTCUTS{{tr::keycode::LEFT}};
// Shortcuts of the page increment button.
constexpr std::initializer_list<key_chord> PAGE_INC_SHORTCUTS{{tr::keycode::RIGHT}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

scoreboards_state::scoreboards_state(std::unique_ptr<game>&& game)
	: _substate{substate::IN_SCORES}, _page{0}, _timer{0}, _game{std::move(game)}, _current{scorefile.categories.begin()}
{
	// STATUS CALLBACKS

	const status_callback status_cb{[this] { return _substate == substate::IN_SCORES; }};
	const status_callback gamemode_change_status_cb{
		[this] { return _substate == substate::IN_SCORES && scorefile.categories.size() != 1; }};
	const status_callback page_dec_status_cb{[this] { return _substate == substate::IN_SCORES && _page > 0; }};
	const status_callback page_inc_status_cb{[this] {
		return _substate == substate::IN_SCORES && _page < (std::max(_current->scores.size() - 1, std::size_t{0}) / SCORES_PER_PAGE);
	}};

	// ACTION CALLBACKS

	const action_callback exit_action_cb{[this] {
		_substate = substate::EXITING_TO_TITLE;
		_timer = 0;
		set_up_exit_animation();
	}};
	const action_callback gamemode_dec_action_cb{[this] {
		_substate = substate::SWITCHING_PAGE;
		_timer = 0;
		_page = 0;
		if (_current == scorefile.categories.begin()) {
			_current = scorefile.categories.end();
		}
		--_current;
		set_up_page_switch_animation();
	}};
	const action_callback gamemode_inc_action_cb{[this] {
		_substate = substate::SWITCHING_PAGE;
		_timer = 0;
		_page = 0;
		if (++_current == scorefile.categories.end()) {
			_current = scorefile.categories.begin();
		}
		set_up_page_switch_animation();
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

	// TEXT CALLBACKS

	const text_callback player_info_text_cb{[](auto&) {
		return std::format("{} {}: {}:{:02}:{:02}", localization["total_playtime"], scorefile.name,
						   scorefile.playtime / (SECOND_TICKS * 3600), (scorefile.playtime % (SECOND_TICKS * 3600)) / (SECOND_TICKS * 60),
						   (scorefile.playtime % (SECOND_TICKS * 60) / SECOND_TICKS));
	}};
	const text_callback cur_gamemode_text_cb{[this](auto&) { return std::string{_current->gamemode.name_loc()}; }};
	const text_callback cur_page_text_cb{[this](auto&) {
		return std::format("{}/{}", _page + 1, std::max(_current->scores.size() - 1, std::size_t{0}) / SCORES_PER_PAGE + 1);
	}};

	//

	widget& title{_ui.emplace<text_widget>("scoreboards", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	widget& player_info{_ui.emplace<text_widget>("player_info", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL,
												 32, player_info_text_cb)};
	player_info.pos.change({500, 64}, 0.5_s);
	player_info.unhide(0.5_s);

	widget& exit{_ui.emplace<clickable_text_widget>("exit", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													DEFAULT_TEXT_CALLBACK, status_cb, exit_action_cb, NO_TOOLTIP, EXIT_SHORTCUTS,
													sfx::CANCEL)};
	exit.pos.change({500, 1000}, 0.5_s);
	exit.unhide(0.5_s);

	if (scorefile.categories.empty()) {
		widget& no_scores_found{_ui.emplace<text_widget>("no_scores_found", glm::vec2{600, 483}, tr::align::TOP_CENTER, font::LANGUAGE,
														 tr::ttf_style::NORMAL, 64, DEFAULT_TEXT_CALLBACK, "80808080"_rgba8)};
		no_scores_found.pos.change({500, 483}, 0.5_s);
		no_scores_found.unhide(0.5_s);
		return;
	}

	for (std::size_t i = 0; i < SCORES_PER_PAGE; ++i) {
		widget& widget{_ui.emplace<score_widget>(std::format("score{}", i), glm::vec2{i % 2 == 0 ? 400 : 600, 173 + 86 * i},
												 tr::align::CENTER, _page * SCORES_PER_PAGE + i + 1,
												 _current->scores.size() > i ? std::to_address(_current->scores.begin() + i) : nullptr)};
		widget.pos.change({500, 173 + 86 * i}, 0.5_s);
		widget.unhide(0.5_s);
	}

	const tooltip_callback cur_gamemode_tooltip_cb{[this] { return std::string{_current->gamemode.description_loc()}; }};
	widget& cur_gamemode{_ui.emplace<text_widget>("cur_gamemode", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, true, cur_gamemode_tooltip_cb,
												  false, std::vector<key_chord>{}, font::LANGUAGE, tr::ttf_style::NORMAL,
												  tr::halign::CENTER, 48, tr::UNLIMITED_WIDTH, "A0A0A0A0"_rgba8, cur_gamemode_text_cb)};
	cur_gamemode.pos.change({500, 900}, 0.5_s);
	cur_gamemode.unhide(0.5_s);

	widget& gamemode_dec{_ui.emplace<arrow_widget>("gamemode_dec", glm::vec2{-50, 892.5}, tr::align::BOTTOM_LEFT, false,
												   gamemode_change_status_cb, gamemode_dec_action_cb, GAMEMODE_DEC_SHORTCUTS)};
	gamemode_dec.pos.change({10, 892.5}, 0.5_s);
	gamemode_dec.unhide(0.5_s);

	widget& gamemode_inc{_ui.emplace<arrow_widget>("gamemode_inc", glm::vec2{1050, 892.5}, tr::align::BOTTOM_RIGHT, true,
												   gamemode_change_status_cb, gamemode_inc_action_cb, GAMEMODE_INC_SHORTCUTS)};
	gamemode_inc.pos.change({990, 892.5}, 0.5_s);
	gamemode_inc.unhide(0.5_s);

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

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> scoreboards_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> scoreboards_state::update(tr::duration)
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
			for (std::size_t i = 0; i < SCORES_PER_PAGE; ++i) {
				const bool nonempty{_current->scores.size() > _page * SCORES_PER_PAGE + i};
				score_widget& widget{_ui.get<score_widget>(std::format("score{}", i))};
				widget.rank = _page * SCORES_PER_PAGE + i + 1;
				widget.score = nonempty ? std::to_address(_current->scores.begin() + _page * SCORES_PER_PAGE + i) : nullptr;
				widget.pos = {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y};
				widget.pos.change({500, glm::vec2{widget.pos}.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
		return nullptr;
	case substate::EXITING_TO_TITLE:
		return _timer >= 0.5_s ? std::make_unique<title_state>(std::move(_game)) : nullptr;
	}
}

void scoreboards_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void scoreboards_state::set_up_page_switch_animation() noexcept
{
	for (std::size_t i = 0; i < SCORES_PER_PAGE; i++) {
		widget& widget{_ui.get(std::format("score{}", i))};
		widget.pos.change({i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
}

void scoreboards_state::set_up_exit_animation() noexcept
{
	_ui.get("scoreboards").pos.change(TOP_START_POS, 0.5_s);
	_ui.get("player_info").pos.change(TOP_START_POS, 0.5_s);
	_ui.get("exit").pos.change(BOTTOM_START_POS, 0.5_s);
	if (scorefile.categories.empty()) {
		_ui.get("no_scores_found").pos.change({400, 483}, 0.5_s);
	}
	else {
		for (std::size_t i = 0; i < SCORES_PER_PAGE; i++) {
			widget& widget{_ui.get(std::format("score{}", i))};
			widget.pos.change({i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.5_s);
		}
		_ui.get("cur_gamemode").pos.change(BOTTOM_START_POS, 0.5_s);
		_ui.get("gamemode_dec").pos.change({-50, 892.5}, 0.5_s);
		_ui.get("gamemode_inc").pos.change({1050, 892.5}, 0.5_s);
		_ui.get("cur_page").pos.change(BOTTOM_START_POS, 0.5_s);
		_ui.get("page_dec").pos.change({-50, 942.5}, 0.5_s);
		_ui.get("page_inc").pos.change({1050, 942.5}, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}