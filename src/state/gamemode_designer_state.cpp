#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/ball_settings_editor_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/player_settings_editor_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag TAG_TITLE{"gamemode_designer"};
constexpr tag TAG_NAME{"name"};
constexpr tag TAG_AUTHOR{"author"};
constexpr tag TAG_DESCRIPTION{"description"};
constexpr tag TAG_BALL_SETTINGS{"ball_settings"};
constexpr tag TAG_PLAYER_SETTINGS{"player_settings"};
constexpr tag TAG_TEST{"test"};
constexpr tag TAG_SAVE{"save"};
constexpr tag TAG_DISCARD{"discard"};

// Buttons at the bottom of the screen.
constexpr std::array<tag, 3> BOTTOM_BUTTONS{TAG_TEST, TAG_SAVE, TAG_DISCARD};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::B}, TAG_BALL_SETTINGS},
	{{tr::system::keycode::TOP_ROW_1}, TAG_BALL_SETTINGS},
	{{tr::system::keycode::P}, TAG_PLAYER_SETTINGS},
	{{tr::system::keycode::TOP_ROW_2}, TAG_PLAYER_SETTINGS},
	{{tr::system::keycode::T}, TAG_TEST},
	{{tr::system::keycode::TOP_ROW_3}, TAG_TEST},
	{{tr::system::keycode::S}, TAG_SAVE},
	{{tr::system::keycode::ENTER}, TAG_SAVE},
	{{tr::system::keycode::TOP_ROW_4}, TAG_SAVE},
	{{tr::system::keycode::C}, TAG_DISCARD},
	{{tr::system::keycode::Q}, TAG_DISCARD},
	{{tr::system::keycode::E}, TAG_DISCARD},
	{{tr::system::keycode::ESCAPE}, TAG_DISCARD},
	{{tr::system::keycode::TOP_ROW_5}, TAG_DISCARD},
};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

gamemode_designer_state::gamemode_designer_state(std::unique_ptr<game>&& game, const gamemode& gamemode, bool returning_from_subscreen)
	: m_substate{substate::IN_GAMEMODE_DESIGNER}, m_timer{0}, m_ui{SHORTCUTS}, m_background_game{std::move(game)}, m_pending{gamemode}
{
	set_up_ui(returning_from_subscreen);
}

gamemode_designer_state::gamemode_designer_state(const gamemode& gamemode)
	: m_substate{substate::RETURNING_FROM_TEST_GAME}
	, m_timer{0}
	, m_ui{SHORTCUTS}
	, m_background_game{std::make_unique<game>(pick_menu_gamemode(), engine::rng.generate<std::uint64_t>())}
	, m_pending{gamemode}
{
	set_up_ui(false);
	engine::play_song("menu", SKIP_MENU_SONG_INTRO, 0.5s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> gamemode_designer_state::handle_event(const tr::system::event& event)
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
	tr::gfx::renderer_2d::draw(engine::screen());
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
		[this] { return m_substate == substate::IN_GAMEMODE_DESIGNER && !m_ui.as<line_input_widget<12>>(TAG_NAME).buffer.empty(); },
		status_cb,
	};

	// ACTION CALLBACKS

	const action_callback name_enter_cb{[this] { m_ui.move_input_focus_forward(); }};
	const action_callback description_enter_cb{[this] { m_ui.clear_input_focus(); }};
	const action_callback ball_settings_action_cb{[this] {
		m_substate = substate::ENTERING_BALL_SETTINGS_EDITOR;
		m_timer = 0;
		m_pending.name = m_ui.as<line_input_widget<12>>(TAG_NAME).buffer;
		m_pending.description = m_ui.as<line_input_widget<40>>(TAG_DESCRIPTION).buffer;
		set_up_subscreen_animation();
	}};
	const action_callback player_settings_action_cb{[this] {
		m_substate = substate::ENTERING_PLAYER_SETTINGS_EDITOR;
		m_timer = 0;
		m_pending.name = m_ui.as<line_input_widget<12>>(TAG_NAME).buffer;
		m_pending.description = m_ui.as<line_input_widget<40>>(TAG_DESCRIPTION).buffer;
		set_up_subscreen_animation();
	}};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_action_cbs{
		[this] {
			m_substate = substate::ENTERING_TEST_GAME;
			m_timer = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(TAG_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(TAG_DESCRIPTION).buffer;
			set_up_exit_animation();
			engine::fade_song_out(0.5s);
		},
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(TAG_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(TAG_DESCRIPTION).buffer;
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

	text_callback author_text_cb{string_text_callback{std::format("{}: {}", engine::loc["by"], engine::scorefile.name)}};

	//

	if (returning_from_subscreen) {
		widget& title{m_ui.emplace<text_widget>(TAG_TITLE, TITLE_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL,
												64, loc_text_callback{TAG_TITLE})};
		title.unhide();
	}
	else {
		widget& title{m_ui.emplace<text_widget>(TAG_TITLE, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE,
												tr::system::ttf_style::NORMAL, 64, loc_text_callback{TAG_TITLE})};
		title.pos.change(interp_mode::CUBE, TITLE_POS, 0.5_s);
		title.unhide(0.5_s);
	}

	line_input_widget<12>& name{m_ui.emplace<line_input_widget<12>>(TAG_NAME, glm::vec2{400, 340}, tr::align::CENTER,
																	tr::system::ttf_style::NORMAL, 120, status_cb, name_enter_cb)};
	name.buffer = m_pending.name;
	name.pos.change(interp_mode::CUBE, {500, 340}, 0.5_s);
	name.unhide(0.5_s);

	widget& author{m_ui.emplace<text_widget>(TAG_AUTHOR, glm::vec2{600, 415}, tr::align::CENTER, font::LANGUAGE,
											 tr::system::ttf_style::NORMAL, 32, std::move(author_text_cb))};
	author.pos.change(interp_mode::CUBE, {500, 415}, 0.5_s);
	author.unhide(0.5_s);

	line_input_widget<40>& description{m_ui.emplace<line_input_widget<40>>(
		TAG_DESCRIPTION, glm::vec2{400, 465}, tr::align::CENTER, tr::system::ttf_style::ITALIC, 32, status_cb, description_enter_cb)};
	description.buffer = m_pending.description;
	description.pos.change(interp_mode::CUBE, {500, 465}, 0.5_s);
	description.unhide(0.5_s);

	widget& ball_settings{m_ui.emplace<clickable_text_widget>(TAG_BALL_SETTINGS, glm::vec2{600, 550}, tr::align::CENTER, font::LANGUAGE, 64,
															  loc_text_callback{TAG_BALL_SETTINGS}, status_cb, ball_settings_action_cb)};
	ball_settings.pos.change(interp_mode::CUBE, {500, 550}, 0.5_s);
	ball_settings.unhide(0.5_s);

	widget& player_settings{m_ui.emplace<clickable_text_widget>(TAG_PLAYER_SETTINGS, glm::vec2{400, 650}, tr::align::CENTER, font::LANGUAGE,
																64, loc_text_callback{TAG_PLAYER_SETTINGS}, status_cb,
																player_settings_action_cb)};
	player_settings.pos.change(interp_mode::CUBE, {500, 650}, 0.5_s);
	player_settings.unhide(0.5_s);

	for (std::size_t i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sound sound{i != BOTTOM_BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL};
		widget& widget{m_ui.emplace<clickable_text_widget>(BOTTOM_BUTTONS[i], BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE,
														   48, loc_text_callback{BOTTOM_BUTTONS[i]}, bottom_status_cbs[i],
														   bottom_action_cbs[i], NO_TOOLTIP, sound)};
		widget.pos.change(interp_mode::CUBE, {500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

void gamemode_designer_state::set_up_subscreen_animation()
{
	widget& name{m_ui[TAG_NAME]};
	widget& author{m_ui[TAG_AUTHOR]};
	widget& description{m_ui[TAG_DESCRIPTION]};
	widget& ball_settings{m_ui[TAG_BALL_SETTINGS]};
	widget& player_settings{m_ui[TAG_PLAYER_SETTINGS]};
	name.pos.change(interp_mode::CUBE, {600, glm::vec2{name.pos}.y}, 0.5_s);
	name.hide(0.5_s);
	author.pos.change(interp_mode::CUBE, {400, glm::vec2{author.pos}.y}, 0.5_s);
	author.hide(0.5_s);
	description.pos.change(interp_mode::CUBE, {600, glm::vec2{description.pos}.y}, 0.5_s);
	description.hide(0.5_s);
	ball_settings.pos.change(interp_mode::CUBE, {400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	ball_settings.hide(0.5_s);
	player_settings.pos.change(interp_mode::CUBE, {600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	player_settings.hide(0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		widget& widget{m_ui[tag]};
		widget.pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
		widget.hide(0.5_s);
	}
}

void gamemode_designer_state::set_up_exit_animation()
{
	m_ui[TAG_TITLE].pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	widget& name{m_ui[TAG_NAME]};
	widget& author{m_ui[TAG_AUTHOR]};
	widget& description{m_ui[TAG_DESCRIPTION]};
	widget& ball_settings{m_ui[TAG_BALL_SETTINGS]};
	widget& player_settings{m_ui[TAG_PLAYER_SETTINGS]};
	name.pos.change(interp_mode::CUBE, {600, glm::vec2{name.pos}.y}, 0.5_s);
	author.pos.change(interp_mode::CUBE, {400, glm::vec2{author.pos}.y}, 0.5_s);
	description.pos.change(interp_mode::CUBE, {600, glm::vec2{description.pos}.y}, 0.5_s);
	ball_settings.pos.change(interp_mode::CUBE, {400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	player_settings.pos.change(interp_mode::CUBE, {600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	}
	m_ui.hide_all(0.5_s);
}