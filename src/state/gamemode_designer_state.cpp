#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/ball_settings_editor_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/player_settings_editor_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Shortcuts of the ball settings button.
constexpr std::initializer_list<tr::key_chord> BALL_SETTINGS_SHORTCUTS{{tr::keycode::B}, {tr::keycode::TOP_ROW_1}};
// Shortcuts of the player settings button.
constexpr std::initializer_list<tr::key_chord> PLAYER_SETTINGS_SHORTCUTS{{tr::keycode::P}, {tr::keycode::TOP_ROW_2}};
// Buttons at the bottom of the screen.
constexpr std::array<const char*, 3> BOTTOM_BUTTONS{"test", "save", "discard"};
// Shortcuts of the bottom buttons.
constexpr std::array<std::initializer_list<tr::key_chord>, BOTTOM_BUTTONS.size()> BOTTOM_SHORTCUTS{{
	{{tr::keycode::T}, {tr::keycode::TOP_ROW_3}},
	{{tr::keycode::S}, {tr::keycode::ENTER}, {tr::keycode::TOP_ROW_4}},
	{{tr::keycode::C}, {tr::keycode::Q}, {tr::keycode::E}, {tr::keycode::ESCAPE}, {tr::keycode::TOP_ROW_5}},
}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

gamemode_designer_state::gamemode_designer_state(std::unique_ptr<game>&& game, const gamemode& gamemode, bool returning_from_subscreen)
	: _substate{substate::IN_GAMEMODE_DESIGNER}, _timer{0}, _game{std::move(game)}, _gamemode{gamemode}
{
	set_up_ui(returning_from_subscreen);
}

gamemode_designer_state::gamemode_designer_state(const gamemode& gamemode)
	: _substate{substate::RETURNING_FROM_TEST_GAME}
	, _timer{0}
	, _game{std::make_unique<game>(MENU_GAMEMODES[tr::rand(rng, MENU_GAMEMODES.size())], tr::rand<std::uint64_t>(rng))}
	, _gamemode{gamemode}
{
	set_up_ui(false);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> gamemode_designer_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> gamemode_designer_state::update(tr::duration)
{
	++_timer;
	_game->update({});
	_ui.update();

	switch (_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		if (_timer >= 0.5_s) {
			_substate = substate::IN_GAMEMODE_DESIGNER;
			_timer = 0;
		}
		return nullptr;
	case substate::IN_GAMEMODE_DESIGNER:
		return nullptr;
	case substate::ENTERING_TEST_GAME:
		return _timer >= 0.5_s ? std::make_unique<game_state>(std::make_unique<active_game>(_gamemode), game_type::TEST, true) : nullptr;
	case substate::ENTERING_BALL_SETTINGS_EDITOR:
		return _timer >= 0.5_s ? std::make_unique<ball_settings_editor_state>(std::move(_game), _gamemode) : nullptr;
	case substate::ENTERING_PLAYER_SETTINGS_EDITOR:
		return _timer >= 0.5_s ? std::make_unique<player_settings_editor_state>(std::move(_game), _gamemode) : nullptr;
	case substate::ENTERING_TITLE:
		return _timer >= 0.5_s ? std::make_unique<title_state>(std::move(_game)) : nullptr;
	}
}

void gamemode_designer_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float gamemode_designer_state::fade_overlay_opacity() const noexcept
{
	switch (_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		return 1 - _timer / 0.5_sf;
	case substate::IN_GAMEMODE_DESIGNER:
	case substate::ENTERING_BALL_SETTINGS_EDITOR:
	case substate::ENTERING_PLAYER_SETTINGS_EDITOR:
	case substate::ENTERING_TITLE:
		return 0;
	case substate::ENTERING_TEST_GAME:
		return _timer / 0.5_sf;
	}
}

void gamemode_designer_state::set_up_ui(bool returning_from_subscreen)
{
	// STATUS CALLBACKS

	const status_callback status_cb{[this] { return _substate == substate::IN_GAMEMODE_DESIGNER; }};
	const std::array<status_callback, BOTTOM_BUTTONS.size()> bottom_status_cbs{
		status_cb,
		[this] { return _substate == substate::IN_GAMEMODE_DESIGNER && !_ui.get<line_input_widget<12>>("name").buffer.empty(); },
		status_cb,
	};

	// ACTION CALLBACKS

	const action_callback name_enter_cb{[this] { _ui.move_input_focus_forward(); }};
	const action_callback description_enter_cb{[this] { _ui.clear_input_focus(); }};
	const action_callback ball_settings_action_cb{[this] {
		_substate = substate::ENTERING_BALL_SETTINGS_EDITOR;
		_timer = 0;
		_gamemode.name = _ui.get<line_input_widget<12>>("name").buffer;
		_gamemode.description = _ui.get<line_input_widget<40>>("description").buffer;
		set_up_subscreen_animation();
	}};
	const action_callback player_settings_action_cb{[this] {
		_substate = substate::ENTERING_PLAYER_SETTINGS_EDITOR;
		_timer = 0;
		_gamemode.name = _ui.get<line_input_widget<12>>("name").buffer;
		_gamemode.description = _ui.get<line_input_widget<40>>("description").buffer;
		set_up_subscreen_animation();
	}};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_action_cbs{
		[this] {
			_substate = substate::ENTERING_TEST_GAME;
			_timer = 0;
			_gamemode.name = _ui.get<line_input_widget<12>>("name").buffer;
			_gamemode.description = _ui.get<line_input_widget<40>>("description").buffer;
			set_up_exit_animation();
		},
		[this] {
			_substate = substate::ENTERING_TITLE;
			_timer = 0;
			_gamemode.name = _ui.get<line_input_widget<12>>("name").buffer;
			_gamemode.description = _ui.get<line_input_widget<40>>("description").buffer;
			_gamemode.save_to_file();
			set_up_exit_animation();
		},
		[this] {
			_substate = substate::ENTERING_TITLE;
			_timer = 0;
			set_up_exit_animation();
		},
	};

	// TEXT CALLBACKS

	text_callback author_text_cb{[str = std::format("{}: {}", localization["by"], scorefile.name)](auto&) { return str; }};

	//

	if (returning_from_subscreen) {
		widget& title{
			_ui.emplace<text_widget>("gamemode_designer", TITLE_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
		title.unhide();
	}
	else {
		widget& title{
			_ui.emplace<text_widget>("gamemode_designer", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
		title.pos.change(TITLE_POS, 0.5_s);
		title.unhide(0.5_s);
	}

	line_input_widget<12>& name{_ui.emplace<line_input_widget<12>>("name", glm::vec2{400, 340}, tr::align::CENTER, tr::ttf_style::NORMAL,
																   120, status_cb, name_enter_cb)};
	name.buffer = _gamemode.name;
	name.pos.change({500, 340}, 0.5_s);
	name.unhide(0.5_s);

	widget& author{_ui.emplace<text_widget>("author", glm::vec2{600, 415}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 32,
											std::move(author_text_cb))};
	author.pos.change({500, 415}, 0.5_s);
	author.unhide(0.5_s);

	line_input_widget<40>& description{_ui.emplace<line_input_widget<40>>("description", glm::vec2{400, 465}, tr::align::CENTER,
																		  tr::ttf_style::ITALIC, 32, status_cb, description_enter_cb)};
	description.buffer = _gamemode.description;
	description.pos.change({500, 465}, 0.5_s);
	description.unhide(0.5_s);

	widget& ball_settings{_ui.emplace<clickable_text_widget>("ball_settings", glm::vec2{600, 550}, tr::align::CENTER, font::LANGUAGE, 64,
															 DEFAULT_TEXT_CALLBACK, status_cb, ball_settings_action_cb, NO_TOOLTIP,
															 BALL_SETTINGS_SHORTCUTS)};
	ball_settings.pos.change({500, 550}, 0.5_s);
	ball_settings.unhide(0.5_s);

	widget& player_settings{_ui.emplace<clickable_text_widget>("player_settings", glm::vec2{400, 650}, tr::align::CENTER, font::LANGUAGE,
															   64, DEFAULT_TEXT_CALLBACK, status_cb, player_settings_action_cb, NO_TOOLTIP,
															   PLAYER_SETTINGS_SHORTCUTS)};
	player_settings.pos.change({500, 650}, 0.5_s);
	player_settings.unhide(0.5_s);

	for (std::size_t i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sfx sfx{i != BOTTOM_BUTTONS.size() - 1 ? sfx::CONFIRM : sfx::CANCEL};
		widget& widget{_ui.emplace<clickable_text_widget>(BOTTOM_BUTTONS[i], BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
														  DEFAULT_TEXT_CALLBACK, bottom_status_cbs[i], bottom_action_cbs[i], NO_TOOLTIP,
														  BOTTOM_SHORTCUTS[i], sfx)};
		widget.pos.change({500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

void gamemode_designer_state::set_up_subscreen_animation() noexcept
{
	widget& name{_ui.get("name")};
	widget& author{_ui.get("author")};
	widget& description{_ui.get("description")};
	widget& ball_settings{_ui.get("ball_settings")};
	widget& player_settings{_ui.get("player_settings")};
	name.pos.change({600, glm::vec2{name.pos}.y}, 0.5_s);
	name.hide(0.5_s);
	author.pos.change({400, glm::vec2{author.pos}.y}, 0.5_s);
	author.hide(0.5_s);
	description.pos.change({600, glm::vec2{description.pos}.y}, 0.5_s);
	description.hide(0.5_s);
	ball_settings.pos.change({400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	ball_settings.hide(0.5_s);
	player_settings.pos.change({600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	player_settings.hide(0.5_s);
	for (const char* tag : BOTTOM_BUTTONS) {
		widget& widget{_ui.get(tag)};
		widget.pos.change(BOTTOM_START_POS, 0.5_s);
		widget.hide(0.5_s);
	}
}

void gamemode_designer_state::set_up_exit_animation() noexcept
{
	_ui.get("gamemode_designer").pos.change(TOP_START_POS, 0.5_s);
	widget& name{_ui.get("name")};
	widget& author{_ui.get("author")};
	widget& description{_ui.get("description")};
	widget& ball_settings{_ui.get("ball_settings")};
	widget& player_settings{_ui.get("player_settings")};
	name.pos.change({600, glm::vec2{name.pos}.y}, 0.5_s);
	author.pos.change({400, glm::vec2{author.pos}.y}, 0.5_s);
	description.pos.change({600, glm::vec2{description.pos}.y}, 0.5_s);
	ball_settings.pos.change({400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	player_settings.pos.change({600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	for (const char* tag : BOTTOM_BUTTONS) {
		_ui.get(tag).pos.change(BOTTOM_START_POS, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}