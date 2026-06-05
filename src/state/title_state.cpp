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
	// clang-format off
	m_ui.emplace<image_widget>(T_LOGO_TEXT, {
		.animation = {{500, 100}, {500, 160}, 2.5_s},
		.unhide_time = 2.5_s,
		.file = "logo_text"
	});
	m_ui.emplace<image_widget>(T_LOGO_OVERLAY, {
		.animation = {{500, 100}, {500, 160}, 2.5_s},
		.unhide_time = 2.5_s,
		.priority = 1,
		.file = "logo_overlay",
		.hue = active_settings::instance()->primary_hue
	});
	m_ui.emplace<image_widget>(T_LOGO_BALL, {
		.animation = {{-180, 644}, {327, 217}, 2.5_s},
		.unhide_time = 2.5_s,
		.priority = 2,
		.file = "logo_ball",
		.hue = active_settings::instance()->secondary_hue
	});

	widget& copyright{m_ui.emplace<label_widget>(T_COPYRIGHT, {
		.animation = {{4, 1000}},
		.alignment = tr::align::TOP_LEFT,
		.unhide_time = 1_s,
		.text = constant_text{T_COPYRIGHT},
		.font_size = 24
	})};
	widget& version{m_ui.emplace<label_widget>(T_VERSION, {
		.animation = {{996, 1000}},
		.alignment = tr::align::TOP_RIGHT,
		.unhide_time = 1_s,
		.tooltip_text = localized_text{"version_tt"},
		.text = constant_text{T_VERSION},
		.font_size = 24
	})};
	copyright.pos.move_y(998 - copyright.size().y, 1_s);
	version.pos.move_y(998 - version.size().y, 1_s);

	struct button_parameters {
		action_command action;
		sound sound;
	};
	const std::array<button_parameters, BUTTONS.size()> button_parameters{{
		{[this] { on_start_game(); }, sound::CONFIRM},
		{[this] { on_gamemode_manager(); }, sound::CONFIRM},
		{[this] { on_scoreboards(); }, sound::CONFIRM},
		{[this] { on_replays(); }, sound::CONFIRM},
		{[this] { on_settings(); }, sound::CONFIRM},
		{[this] { on_credits(); }, sound::CONFIRM},
		{[this] { on_exit(); }, sound::CANCEL},
	}};
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const glm::vec2 end_pos{990 - 25 * i, 965 - (BUTTONS.size() - i - 1) * 50};
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(35.0f, 75.0f)};
		m_ui.emplace<text_button_widget>(BUTTONS[i], {
			.animation = {{end_pos.x + offset, end_pos.y}, end_pos, 1_s},
			.alignment = tr::align::CENTER_RIGHT,
			.unhide_time = 1_s,
			.text = localized_text{BUTTONS[i]},
			.status = [this] { return m_substate == substate::IN_TITLE || m_substate == substate::FADING_IN; },
			.action = button_parameters[i].action,
			.action_sound = button_parameters[i].sound
		});
	}
	// clang-format on
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

//

void title_state::on_start_game()
{
	m_substate = substate::EXITING_TO_SUBMENU;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<start_game_state>(m_game);
}

void title_state::on_gamemode_manager()
{
	m_substate = substate::EXITING_TO_SUBMENU;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<gamemode_manager_state>(m_game, animate_title::YES);
}

void title_state::on_scoreboards()
{
	m_substate = substate::EXITING_TO_SUBMENU;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<scoreboard_selection_state>(m_game, animate_title::YES);
}

void title_state::on_replays()
{
	m_substate = substate::EXITING_TO_SUBMENU;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<replays_state>(m_game);
}

void title_state::on_settings()
{
	m_substate = substate::EXITING_TO_SUBMENU;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<settings_state>(m_game);
}

void title_state::on_credits()
{
	m_substate = substate::EXITING_TO_SUBMENU;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<credits_state>(m_game);
}

void title_state::on_exit()
{
	m_substate = substate::EXITING_GAME;
	m_elapsed = 0;
	set_up_exit_animation();
	g_audio.fade_song_out(0.5s);
}