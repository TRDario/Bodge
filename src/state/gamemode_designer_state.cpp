#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/ball_settings_editor_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/player_settings_editor_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"gamemode_designer"};
constexpr tag T_NAME{"name"};
constexpr tag T_AUTHOR{"author"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_BALL_SETTINGS{"ball_settings"};
constexpr tag T_PLAYER_SETTINGS{"player_settings"};
constexpr tag T_TEST{"test"};
constexpr tag T_SAVE{"save"};
constexpr tag T_DISCARD{"discard"};

constexpr std::array<tag, 3> BOTTOM_BUTTONS{T_TEST, T_SAVE, T_DISCARD};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_NAME},          selection_tree_row{T_DESCRIPTION},
	selection_tree_row{T_BALL_SETTINGS}, selection_tree_row{T_PLAYER_SETTINGS},
	selection_tree_row{T_TEST},          selection_tree_row{T_SAVE},
	selection_tree_row{T_DISCARD},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::B}, T_BALL_SETTINGS},
	{{tr::system::keycode::TOP_ROW_1}, T_BALL_SETTINGS},
	{{tr::system::keycode::P}, T_PLAYER_SETTINGS},
	{{tr::system::keycode::TOP_ROW_2}, T_PLAYER_SETTINGS},
	{{tr::system::keycode::T}, T_TEST},
	{{tr::system::keycode::TOP_ROW_3}, T_TEST},
	{{tr::system::keycode::S}, T_SAVE},
	{{tr::system::keycode::ENTER}, T_SAVE},
	{{tr::system::keycode::TOP_ROW_4}, T_SAVE},
	{{tr::system::keycode::C}, T_DISCARD},
	{{tr::system::keycode::Q}, T_DISCARD},
	{{tr::system::keycode::E}, T_DISCARD},
	{{tr::system::keycode::ESCAPE}, T_DISCARD},
	{{tr::system::keycode::TOP_ROW_5}, T_DISCARD},
};

constexpr interpolator<glm::vec2> TITLE_MOVE_IN{interp::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr interpolator<glm::vec2> NAME_MOVE_IN{interp::CUBIC, glm::vec2{400, 340}, {500, 340}, 0.5_s};
constexpr interpolator<glm::vec2> AUTHOR_MOVE_IN{interp::CUBIC, glm::vec2{600, 415}, glm::vec2{500, 415}, 0.5_s};
constexpr interpolator<glm::vec2> DESCRIPTION_MOVE_IN{interp::CUBIC, glm::vec2{400, 465}, glm::vec2{500, 465}, 0.5_s};
constexpr interpolator<glm::vec2> BALL_SETTINGS_MOVE_IN{interp::CUBIC, glm::vec2{600, 550}, glm::vec2{500, 550}, 0.5_s};
constexpr interpolator<glm::vec2> PLAYER_SETTINGS_MOVE_IN{interp::CUBIC, glm::vec2{400, 650}, glm::vec2{500, 650}, 0.5_s};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

gamemode_designer_state::gamemode_designer_state(std::unique_ptr<game>&& game, const gamemode& gamemode, bool returning_from_subscreen)
	: m_substate{substate::IN_GAMEMODE_DESIGNER}
	, m_timer{0}
	, m_ui{SELECTION_TREE, SHORTCUTS}
	, m_background_game{std::move(game)}
	, m_pending{gamemode}
{
	set_up_ui(returning_from_subscreen);
}

gamemode_designer_state::gamemode_designer_state(const gamemode& gamemode)
	: m_substate{substate::RETURNING_FROM_TEST_GAME}
	, m_timer{0}
	, m_ui{SELECTION_TREE, SHORTCUTS}
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
	const std::array<status_callback, BOTTOM_BUTTONS.size()> bottom_scbs{
		status_cb,
		[this] { return m_substate == substate::IN_GAMEMODE_DESIGNER && !m_ui.as<line_input_widget<12>>(T_NAME).buffer.empty(); },
		status_cb,
	};

	// ACTION CALLBACKS

	const action_callback name_enter_cb{[this] { m_ui.select_next(); }};
	const action_callback description_enter_cb{[this] { m_ui.set_selection(nullptr); }};
	const action_callback ball_settings_acb{[this] {
		m_substate = substate::ENTERING_BALL_SETTINGS_EDITOR;
		m_timer = 0;
		m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
		m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
		set_up_subscreen_animation();
	}};
	const action_callback player_settings_acb{[this] {
		m_substate = substate::ENTERING_PLAYER_SETTINGS_EDITOR;
		m_timer = 0;
		m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
		m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
		set_up_subscreen_animation();
	}};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_acbs{
		[this] {
			m_substate = substate::ENTERING_TEST_GAME;
			m_timer = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			set_up_exit_animation();
			engine::fade_song_out(0.5s);
		},
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
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

	text_callback author_tcb{string_text_callback{std::format("{}: {}", engine::loc["by"], engine::scorefile.name)}};

	//

	if (returning_from_subscreen) {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::system::ttf_style::NORMAL, 64);
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::system::ttf_style::NORMAL, 64);
	}
	m_ui.emplace<line_input_widget<12>>(T_NAME, NAME_MOVE_IN, tr::align::CENTER, 0.5_s, tr::system::ttf_style::NORMAL, 120, status_cb,
										name_enter_cb, m_pending.name);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(author_tcb),
							   tr::system::ttf_style::NORMAL, 32);
	m_ui.emplace<line_input_widget<40>>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, tr::system::ttf_style::ITALIC, 32,
										status_cb, description_enter_cb, m_pending.description);
	m_ui.emplace<clickable_text_widget>(T_BALL_SETTINGS, BALL_SETTINGS_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE, 64,
										loc_text_callback{T_BALL_SETTINGS}, status_cb, ball_settings_acb);
	m_ui.emplace<clickable_text_widget>(T_PLAYER_SETTINGS, PLAYER_SETTINGS_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE, 64,
										loc_text_callback{T_PLAYER_SETTINGS}, status_cb, player_settings_acb);
	for (std::size_t i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sound sound{i != BOTTOM_BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL};
		const interpolator<glm::vec2> move_in{
			interp::CUBIC, BOTTOM_START_POS, {500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s};
		m_ui.emplace<clickable_text_widget>(BOTTOM_BUTTONS[i], move_in, tr::align::BOTTOM_CENTER, 0.5_s, font::LANGUAGE, 48,
											loc_text_callback{BOTTOM_BUTTONS[i]}, bottom_scbs[i], bottom_acbs[i], NO_TOOLTIP, sound);
	}
}

void gamemode_designer_state::set_up_subscreen_animation()
{
	widget& name{m_ui[T_NAME]};
	widget& author{m_ui[T_AUTHOR]};
	widget& description{m_ui[T_DESCRIPTION]};
	widget& ball_settings{m_ui[T_BALL_SETTINGS]};
	widget& player_settings{m_ui[T_PLAYER_SETTINGS]};
	name.pos.change(interp::CUBIC, {600, glm::vec2{name.pos}.y}, 0.5_s);
	name.hide(0.5_s);
	author.pos.change(interp::CUBIC, {400, glm::vec2{author.pos}.y}, 0.5_s);
	author.hide(0.5_s);
	description.pos.change(interp::CUBIC, {600, glm::vec2{description.pos}.y}, 0.5_s);
	description.hide(0.5_s);
	ball_settings.pos.change(interp::CUBIC, {400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	ball_settings.hide(0.5_s);
	player_settings.pos.change(interp::CUBIC, {600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	player_settings.hide(0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		widget& widget{m_ui[tag]};
		widget.pos.change(interp::CUBIC, BOTTOM_START_POS, 0.5_s);
		widget.hide(0.5_s);
	}
}

void gamemode_designer_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(interp::CUBIC, TOP_START_POS, 0.5_s);
	widget& name{m_ui[T_NAME]};
	widget& author{m_ui[T_AUTHOR]};
	widget& description{m_ui[T_DESCRIPTION]};
	widget& ball_settings{m_ui[T_BALL_SETTINGS]};
	widget& player_settings{m_ui[T_PLAYER_SETTINGS]};
	name.pos.change(interp::CUBIC, {600, glm::vec2{name.pos}.y}, 0.5_s);
	author.pos.change(interp::CUBIC, {400, glm::vec2{author.pos}.y}, 0.5_s);
	description.pos.change(interp::CUBIC, {600, glm::vec2{description.pos}.y}, 0.5_s);
	ball_settings.pos.change(interp::CUBIC, {400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	player_settings.pos.change(interp::CUBIC, {600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].pos.change(interp::CUBIC, BOTTOM_START_POS, 0.5_s);
	}
	m_ui.hide_all(0.5_s);
}