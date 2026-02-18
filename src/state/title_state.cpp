///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements title_state from state.hpp.                                                                                                //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/audio.hpp"
#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_LOGO_TEXT{"logo_text"};
constexpr tag T_LOGO_OVERLAY{"logo_overlay"};
constexpr tag T_LOGO_BALL{"logo_ball"};
constexpr tag T_COPYRIGHT{"© 2025-2026 TRDario"};
constexpr tag T_VERSION{VERSION_STRING};
constexpr tag T_START_GAME{"start_game"};
constexpr tag T_GAMEMODE_MANAGER{"gamemode_manager"};
constexpr tag T_SCOREBOARDS{"scoreboards"};
constexpr tag T_REPLAYS{"replays"};
constexpr tag T_SETTINGS{"settings"};
constexpr tag T_CREDITS{"credits"};
constexpr tag T_EXIT{"exit"};

// Title screen buttons.
constexpr std::array BUTTONS{T_START_GAME, T_GAMEMODE_MANAGER, T_SCOREBOARDS, T_REPLAYS, T_SETTINGS, T_CREDITS, T_EXIT};

// Selection tree for the title screen.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_START_GAME},
	selection_tree_row{T_GAMEMODE_MANAGER},
	selection_tree_row{T_SCOREBOARDS},
	selection_tree_row{T_REPLAYS},
	selection_tree_row{T_SETTINGS},
	selection_tree_row{T_CREDITS},
	selection_tree_row{T_EXIT},
};

// Shortcut table for the title screen.
constexpr shortcut_table SHORTCUTS{
	{"Enter"_kc, T_START_GAME}, {"1"_kc, T_START_GAME},
	{"G"_kc, T_GAMEMODE_MANAGER}, {"2"_kc, T_GAMEMODE_MANAGER},
	{"B"_kc, T_SCOREBOARDS}, {"3"_kc, T_SCOREBOARDS},
	{"R"_kc, T_REPLAYS}, {"4"_kc, T_REPLAYS},
	{"S"_kc, T_SETTINGS}, {"5"_kc, T_SETTINGS},
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT}, {"6"_kc, T_EXIT},
};

// Entry animation for logo text widgets.
constexpr tweened_position LOGO_TEXT_ANIMATION{{500, 100}, {500, 160}, 2.5_s};
// Entry animation for the logo ball widget.
constexpr tweened_position LOGO_BALL_ANIMATION{{-180, 644}, {327, 217}, 2.5_s};

// clang-format on
/////////////////////////////////////////////////////////////// TITLE STATE ///////////////////////////////////////////////////////////////

title_state::title_state()
	: main_menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::FADING_IN}
{
	set_up_ui();
}

title_state::title_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_TITLE}
{
	set_up_ui();
}

//

tr::next_state title_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::FADING_IN:
		if (m_elapsed == 1) {
			g_audio.play_song("menu", 1.0s);
		}
		else if (m_elapsed >= 1.0_s) {
			m_elapsed = 0;
			m_substate = substate::IN_TITLE;
		}
		return tr::KEEP_STATE;
	case substate::IN_TITLE:
		return tr::KEEP_STATE;
	case substate::EXITING_TO_SUBMENU:
		return next_state_if_after(0.5_s);
	case substate::EXITING_GAME:
		return m_elapsed >= 0.5_s ? tr::next_state{nullptr} : tr::KEEP_STATE;
	}
}

//

float title_state::fade_overlay_opacity()
{
	switch (m_substate) {
	case substate::FADING_IN:
		return 1 - m_elapsed / 1_sf;
	case substate::IN_TITLE:
	case substate::EXITING_TO_SUBMENU:
		return 0;
	case substate::EXITING_GAME:
		return m_elapsed / 0.5_sf;
	}
}

void title_state::set_up_ui()
{
	m_ui.emplace<image_widget>(T_LOGO_TEXT, LOGO_TEXT_ANIMATION, tr::align::CENTER, 2.5_s, 0, "logo_text");
	m_ui.emplace<image_widget>(T_LOGO_OVERLAY, LOGO_TEXT_ANIMATION, tr::align::CENTER, 2.5_s, 1, "logo_overlay", g_settings.primary_hue);
	m_ui.emplace<image_widget>(T_LOGO_BALL, LOGO_BALL_ANIMATION, tr::align::CENTER, 2.5_s, 2, "logo_ball", g_settings.secondary_hue);

	widget& copyright{m_ui.emplace<label_widget>(T_COPYRIGHT, glm::vec2{4, 1000}, tr::align::TOP_LEFT, 1_s, NO_TOOLTIP,
												 const_text_callback{T_COPYRIGHT}, tr::sys::ttf_style::NORMAL, 24)};
	copyright.pos.move_y(998 - copyright.size().y, 1_s);
	widget& version{m_ui.emplace<label_widget>(T_VERSION, glm::vec2{996, 1000}, tr::align::TOP_RIGHT, 1_s, loc_text_callback{"version_tt"},
											   const_text_callback{T_VERSION}, tr::sys::ttf_style::NORMAL, 24)};
	version.pos.move_y(998 - version.size().y, 1_s);

	const status_callback scb{[this] { return m_substate == substate::IN_TITLE || m_substate == substate::FADING_IN; }};
	const std::array<action_callback, BUTTONS.size()> action_cbs{
		[this] {
			m_substate = substate::EXITING_TO_SUBMENU;
			m_elapsed = 0;
			set_up_exit_animation();
			m_next_state = make_async<start_game_state>(m_game);
		},
		[this] {
			m_substate = substate::EXITING_TO_SUBMENU;
			m_elapsed = 0;
			set_up_exit_animation();
			m_next_state = make_async<gamemode_manager_state>(m_game, animate_title::YES);
		},
		[this] {
			m_substate = substate::EXITING_TO_SUBMENU;
			m_elapsed = 0;
			set_up_exit_animation();
			m_next_state = make_async<scoreboard_selection_state>(m_game, animate_title::YES);
		},
		[this] {
			m_substate = substate::EXITING_TO_SUBMENU;
			m_elapsed = 0;
			set_up_exit_animation();
			m_next_state = make_async<replays_state>(m_game);
		},
		[this] {
			m_substate = substate::EXITING_TO_SUBMENU;
			m_elapsed = 0;
			set_up_exit_animation();
			m_next_state = make_async<settings_state>(m_game);
		},
		[this] {
			m_substate = substate::EXITING_TO_SUBMENU;
			m_elapsed = 0;
			set_up_exit_animation();
			m_next_state = make_async<credits_state>(m_game);
		},
		[this] {
			m_substate = substate::EXITING_GAME;
			m_elapsed = 0;
			set_up_exit_animation();
			g_audio.fade_song_out(0.5s);
		},
	};

	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const glm::vec2 end_pos{990 - 25 * i, 965 - (BUTTONS.size() - i - 1) * 50};
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(35.0f, 75.0f)};
		const tweened_position animation{{end_pos.x + offset, end_pos.y}, end_pos, 1_s};
		m_ui.emplace<text_button_widget>(BUTTONS[i], animation, tr::align::CENTER_RIGHT, 1_s, NO_TOOLTIP, loc_text_callback{BUTTONS[i]},
										 font::LANGUAGE, 48, scb, action_cbs[i], i != BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL);
	}
}

void title_state::set_up_exit_animation()
{
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const float x{990.0f - 25 * i};
		const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * g_rng.generate(35.0f, 75.0f)};
		m_ui[BUTTONS[i]].move_x_and_hide(x + offset, 0.5_s);
	}
	m_ui[T_LOGO_TEXT].move_y_and_hide(220, 0.5_s);
	m_ui[T_LOGO_OVERLAY].move_y_and_hide(220, 0.5_s);
	m_ui[T_LOGO_BALL].move_and_hide({487, 57}, 0.5_s);
	m_ui[T_COPYRIGHT].move_y_and_hide(1000, 0.5_s);
	m_ui[T_VERSION].move_y_and_hide(1000, 0.5_s);
}