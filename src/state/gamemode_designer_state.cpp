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
	: m_substate{substate::IN_GAMEMODE_DESIGNER}, m_timer{0}, m_background_game{std::move(game)}, m_pending{gamemode}
{
	set_up_ui(returning_from_subscreen);
}

gamemode_designer_state::gamemode_designer_state(const gamemode& gamemode)
	: m_substate{substate::RETURNING_FROM_TEST_GAME}
	, m_timer{0}
	, m_background_game{std::make_unique<game>(pick_menu_gamemode(), engine::rng.generate<std::uint64_t>())}
	, m_pending{gamemode}
{
	set_up_ui(false);
	engine::play_song(song::MENU, SKIP_MENU_SONG_INTRO, 0.5s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> gamemode_designer_state::handle_event(const tr::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> gamemode_designer_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_GAMEMODE_DESIGNER;
			m_timer = 0;
		}
		return nullptr;
	case substate::IN_GAMEMODE_DESIGNER:
		return nullptr;
	case substate::ENTERING_TEST_GAME:
		return m_timer >= 0.5_s ? std::make_unique<game_state>(std::make_unique<active_game>(m_pending), game_type::TEST, true) : nullptr;
	case substate::ENTERING_BALL_SETTINGS_EDITOR:
		return m_timer >= 0.5_s ? std::make_unique<ball_settings_editor_state>(std::move(m_background_game), m_pending) : nullptr;
	case substate::ENTERING_PLAYER_SETTINGS_EDITOR:
		return m_timer >= 0.5_s ? std::make_unique<player_settings_editor_state>(std::move(m_background_game), m_pending) : nullptr;
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? std::make_unique<title_state>(std::move(m_background_game)) : nullptr;
	}
}

void gamemode_designer_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float gamemode_designer_state::fade_overlay_opacity() const
{
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		return 1 - m_timer / 0.5_sf;
	case substate::IN_GAMEMODE_DESIGNER:
	case substate::ENTERING_BALL_SETTINGS_EDITOR:
	case substate::ENTERING_PLAYER_SETTINGS_EDITOR:
	case substate::ENTERING_TITLE:
		return 0;
	case substate::ENTERING_TEST_GAME:
		return m_timer / 0.5_sf;
	}
}

void gamemode_designer_state::set_up_ui(bool returning_from_subscreen)
{
	// STATUS CALLBACKS

	const status_callback status_cb{[this] { return m_substate == substate::IN_GAMEMODE_DESIGNER; }};
	const std::array<status_callback, BOTTOM_BUTTONS.size()> bottom_status_cbs{
		status_cb,
		[this] { return m_substate == substate::IN_GAMEMODE_DESIGNER && !m_ui.get<line_input_widget<12>>("name").buffer.empty(); },
		status_cb,
	};

	// ACTION CALLBACKS

	const action_callback name_enter_cb{[this] { m_ui.move_input_focus_forward(); }};
	const action_callback description_enter_cb{[this] { m_ui.clear_input_focus(); }};
	const action_callback ball_settings_action_cb{[this] {
		m_substate = substate::ENTERING_BALL_SETTINGS_EDITOR;
		m_timer = 0;
		m_pending.name = m_ui.get<line_input_widget<12>>("name").buffer;
		m_pending.description = m_ui.get<line_input_widget<40>>("description").buffer;
		set_up_subscreen_animation();
	}};
	const action_callback player_settings_action_cb{[this] {
		m_substate = substate::ENTERING_PLAYER_SETTINGS_EDITOR;
		m_timer = 0;
		m_pending.name = m_ui.get<line_input_widget<12>>("name").buffer;
		m_pending.description = m_ui.get<line_input_widget<40>>("description").buffer;
		set_up_subscreen_animation();
	}};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_action_cbs{
		[this] {
			m_substate = substate::ENTERING_TEST_GAME;
			m_timer = 0;
			m_pending.name = m_ui.get<line_input_widget<12>>("name").buffer;
			m_pending.description = m_ui.get<line_input_widget<40>>("description").buffer;
			set_up_exit_animation();
			engine::fade_song_out(0.5s);
		},
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			m_pending.name = m_ui.get<line_input_widget<12>>("name").buffer;
			m_pending.description = m_ui.get<line_input_widget<40>>("description").buffer;
			save_gamemode(m_pending);
			set_up_exit_animation();
		},
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			set_up_exit_animation();
		},
	};

	// TEXT CALLBACKS

	text_callback author_text_cb{[str = std::format("{}: {}", engine::loc["by"], engine::scorefile.name)](auto&) { return str; }};

	//

	if (returning_from_subscreen) {
		widget& title{
			m_ui.emplace<text_widget>("gamemode_designer", TITLE_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
		title.unhide();
	}
	else {
		widget& title{m_ui.emplace<text_widget>("gamemode_designer", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE,
												tr::ttf_style::NORMAL, 64)};
		title.pos.change(TITLE_POS, 0.5_s);
		title.unhide(0.5_s);
	}

	line_input_widget<12>& name{m_ui.emplace<line_input_widget<12>>("name", glm::vec2{400, 340}, tr::align::CENTER, tr::ttf_style::NORMAL,
																	120, status_cb, name_enter_cb)};
	name.buffer = m_pending.name;
	name.pos.change({500, 340}, 0.5_s);
	name.unhide(0.5_s);

	widget& author{m_ui.emplace<text_widget>("author", glm::vec2{600, 415}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 32,
											 std::move(author_text_cb))};
	author.pos.change({500, 415}, 0.5_s);
	author.unhide(0.5_s);

	line_input_widget<40>& description{m_ui.emplace<line_input_widget<40>>("description", glm::vec2{400, 465}, tr::align::CENTER,
																		   tr::ttf_style::ITALIC, 32, status_cb, description_enter_cb)};
	description.buffer = m_pending.description;
	description.pos.change({500, 465}, 0.5_s);
	description.unhide(0.5_s);

	widget& ball_settings{m_ui.emplace<clickable_text_widget>("ball_settings", glm::vec2{600, 550}, tr::align::CENTER, font::LANGUAGE, 64,
															  DEFAULT_TEXT_CALLBACK, status_cb, ball_settings_action_cb, NO_TOOLTIP,
															  BALL_SETTINGS_SHORTCUTS)};
	ball_settings.pos.change({500, 550}, 0.5_s);
	ball_settings.unhide(0.5_s);

	widget& player_settings{m_ui.emplace<clickable_text_widget>("player_settings", glm::vec2{400, 650}, tr::align::CENTER, font::LANGUAGE,
																64, DEFAULT_TEXT_CALLBACK, status_cb, player_settings_action_cb, NO_TOOLTIP,
																PLAYER_SETTINGS_SHORTCUTS)};
	player_settings.pos.change({500, 650}, 0.5_s);
	player_settings.unhide(0.5_s);

	for (std::size_t i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sound sound{i != BOTTOM_BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL};
		widget& widget{m_ui.emplace<clickable_text_widget>(BOTTOM_BUTTONS[i], BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE,
														   48, DEFAULT_TEXT_CALLBACK, bottom_status_cbs[i], bottom_action_cbs[i],
														   NO_TOOLTIP, BOTTOM_SHORTCUTS[i], sound)};
		widget.pos.change({500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

void gamemode_designer_state::set_up_subscreen_animation()
{
	widget& name{m_ui.get("name")};
	widget& author{m_ui.get("author")};
	widget& description{m_ui.get("description")};
	widget& ball_settings{m_ui.get("ball_settings")};
	widget& player_settings{m_ui.get("player_settings")};
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
		widget& widget{m_ui.get(tag)};
		widget.pos.change(BOTTOM_START_POS, 0.5_s);
		widget.hide(0.5_s);
	}
}

void gamemode_designer_state::set_up_exit_animation()
{
	m_ui.get("gamemode_designer").pos.change(TOP_START_POS, 0.5_s);
	widget& name{m_ui.get("name")};
	widget& author{m_ui.get("author")};
	widget& description{m_ui.get("description")};
	widget& ball_settings{m_ui.get("ball_settings")};
	widget& player_settings{m_ui.get("player_settings")};
	name.pos.change({600, glm::vec2{name.pos}.y}, 0.5_s);
	author.pos.change({400, glm::vec2{author.pos}.y}, 0.5_s);
	description.pos.change({600, glm::vec2{description.pos}.y}, 0.5_s);
	ball_settings.pos.change({400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	player_settings.pos.change({600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	for (const char* tag : BOTTOM_BUTTONS) {
		m_ui.get(tag).pos.change(BOTTOM_START_POS, 0.5_s);
	}
	m_ui.hide_all(0.5_s);
}