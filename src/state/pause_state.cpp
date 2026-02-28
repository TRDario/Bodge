///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements pause_state from state.hpp.                                                                                                //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_PAUSED{"paused"};
constexpr tag T_TEST_PAUSED{"test_paused"};
constexpr tag T_REPLAY_PAUSED{"replay_paused"};
constexpr tag T_UNPAUSE{"unpause"};
constexpr tag T_SAVE_AND_RESTART{"save_and_restart"};
constexpr tag T_RESTART{"restart"};
constexpr tag T_SAVE_AND_QUIT{"save_and_quit"};
constexpr tag T_QUIT{"quit"};

// List of buttons when pausing in a regular game.
constexpr std::array BUTTONS_REGULAR{T_UNPAUSE, T_SAVE_AND_RESTART, T_RESTART, T_SAVE_AND_QUIT, T_QUIT};
// List of buttons when pausing in a test or replay game.
constexpr std::array BUTTONS_SPECIAL{T_UNPAUSE, T_RESTART, T_QUIT};

// Selection tree for the pause menu when pausing in a regular game.
constexpr selection_tree SELECTION_TREE_REGULAR{
	selection_tree_row{T_UNPAUSE},
	selection_tree_row{T_SAVE_AND_RESTART},
	selection_tree_row{T_RESTART},
	selection_tree_row{T_SAVE_AND_QUIT},
	selection_tree_row{T_QUIT},
};
// Selection tree for the pause menu when pausing in a test or replay game.
constexpr selection_tree SELECTION_TREE_SPECIAL{
	selection_tree_row{T_UNPAUSE},
	selection_tree_row{T_RESTART},
	selection_tree_row{T_QUIT},
};

// Shortcut table for the pause menu when pausing in a regular game.
constexpr shortcut_table SHORTCUTS_REGULAR{
	{"Escape"_kc, T_UNPAUSE}, {"1"_kc, T_UNPAUSE},
	{"Shift+R"_kc, T_SAVE_AND_RESTART}, {"2"_kc, T_SAVE_AND_RESTART},
	{"R"_kc, T_RESTART}, {"3"_kc, T_RESTART},
	{"Shift+Q"_kc, T_SAVE_AND_QUIT}, {"4"_kc, T_SAVE_AND_QUIT},
	{"Q"_kc, T_QUIT}, {"5"_kc, T_QUIT},
};
// Shortcut table for the pause menu when pausing in a test or replay game.
constexpr shortcut_table SHORTCUTS_SPECIAL{
	{"Escape"_kc, T_UNPAUSE}, {"1"_kc, T_UNPAUSE},
	{"R"_kc, T_RESTART}, {"2"_kc, T_RESTART},
	{"Q"_kc, T_QUIT}, {"3"_kc, T_QUIT},
};

// clang-format on
/////////////////////////////////////////////////////////////// PAUSE STATE ///////////////////////////////////////////////////////////////

pause_state::pause_state(std::shared_ptr<game> game, game_state_data data, glm::vec2 mouse_pos, blur_in blur_in)
	: game_menu_state{std::holds_alternative<regular_game_data>(m_data) ? SELECTION_TREE_REGULAR : SELECTION_TREE_SPECIAL,
					  std::holds_alternative<regular_game_data>(m_data) ? SHORTCUTS_REGULAR : SHORTCUTS_SPECIAL, std::move(game),
					  update_game::NO}
	, m_substate{(blur_in == blur_in::YES ? substate::PAUSING : substate::PAUSED)}
	, m_data{std::move(data)}
	, m_start_mouse_pos{mouse_pos}
{
	if (blur_in == blur_in::YES) {
		m_game->add_to_renderer();
		g_renderer->draw_layers(g_renderer->blur.input());
	}

	if (std::holds_alternative<regular_game_data>(m_data)) {
		set_up_full_ui();
	}
	else {
		set_up_limited_ui();
	}
}

//

bool pause_state::transparent_cursor() const
{
	return m_substate == substate::UNPAUSING || m_substate == substate::RESTARTING;
}

tr::next_state pause_state::tick()
{
	game_menu_state::tick();
	switch (m_substate) {
	case substate::PAUSING:
		if (m_elapsed >= 0.5_s) {
			m_elapsed = 0;
			m_substate = substate::PAUSED;
		}
		return tr::KEEP_STATE;
	case substate::PAUSED:
		return tr::KEEP_STATE;
	case substate::UNPAUSING:
		if (!std::holds_alternative<replay_game_data>(m_data)) {
			float ratio{m_elapsed / 0.5_sf};
			ratio = ratio < 0.5 ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
			g_mouse_pos = m_end_mouse_pos + (m_start_mouse_pos - m_end_mouse_pos) * ratio;
		}

		if (m_elapsed >= 0.5_s) {
			g_audio.unpause_song();
			return m_next_state.get();
		}
		else {
			return tr::KEEP_STATE;
		}
	case substate::RESTARTING:
		if (m_elapsed < 0.5_s) {
			return tr::KEEP_STATE;
		}
		g_renderer->set_default_transform(TRANSFORM);
		return m_next_state.get();
	case substate::SAVING:
		return next_state_if_after(0.5_s);
	case substate::QUITTING:
		if (m_elapsed < 0.5_s) {
			return tr::KEEP_STATE;
		}
		g_renderer->set_default_transform(TRANSFORM);
		return m_next_state.get();
	}
}

//

float pause_state::fade_overlay_opacity()
{
	return m_substate == substate::RESTARTING || m_substate == substate::QUITTING ? m_elapsed / 0.5_sf : 0;
}

float pause_state::saturation_factor()
{
	switch (m_substate) {
	case substate::PAUSED:
	case substate::SAVING:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 0.35f;
	case substate::PAUSING:
		return 1 - m_elapsed / 0.5_sf * 0.65f;
	case substate::UNPAUSING:
		return 0.35f + m_elapsed / 0.5_sf * 0.65f;
	}
}

float pause_state::blur_strength()
{
	switch (m_substate) {
	case substate::PAUSED:
	case substate::SAVING:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 10;
	case substate::PAUSING:
		return m_elapsed / 0.5_sf * 10;
	case substate::UNPAUSING:
		return (1 - m_elapsed / 0.5_sf) * 10;
	}
}

void pause_state::set_up_full_ui()
{
	constexpr float TITLE_Y{500.0f - (BUTTONS_REGULAR.size() + 1) * 30};
	constexpr tweened_position TITLE_ANIMATION{{500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};
	m_ui.emplace<label_widget>(T_PAUSED, TITLE_ANIMATION, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_PAUSED},
							   tr::sys::ttf_style::NORMAL, 64);

	const status_callback scb{
		[this] { return m_substate == substate::PAUSED || m_substate == substate::PAUSING; },
	};
	const status_callback unpause_scb{[this] { return m_substate == substate::PAUSED; }};
	std::array<action_callback, BUTTONS_REGULAR.size()> action_cbs{
		[this] {
			m_elapsed = 0;
			m_substate = substate::UNPAUSING;
			m_end_mouse_pos = g_mouse_pos;
			set_up_exit_animation();
			g_audio.play_sound(sound::UNPAUSE, 0.8f, 0.0f);
			m_next_state = make_async<game_state>(m_game, m_data, fade_in::NO);
		},
		[this] {
			m_elapsed = 0;
			m_substate = substate::SAVING;
			set_up_exit_animation();
			m_next_state = make_async<save_score_state>(m_game, m_start_mouse_pos, save_screen_flags::RESTARTING);
		},
		[this] {
			const score_flags score_flags{true, g_cli_settings.game_speed != 1};
			const score_entry score{{}, current_timestamp(), m_game->final_score(), m_game->final_time(), score_flags};

			m_elapsed = 0;
			m_substate = substate::RESTARTING;
			g_scorefile.add_score(m_game->gamemode(), score);
			set_up_exit_animation();
			m_next_state = make_game_state_async<active_game>(m_data, m_game->gamemode());
		},
		[this] {
			m_elapsed = 0;
			m_substate = substate::SAVING;
			set_up_exit_animation();
			m_next_state = make_async<save_score_state>(m_game, m_start_mouse_pos, save_screen_flags::NONE);
		},
		[this] {
			const score_flags score_flags{true, g_cli_settings.game_speed != 1};
			const score_entry score{{}, current_timestamp(), m_game->final_score(), m_game->final_time(), score_flags};

			m_elapsed = 0;
			m_substate = substate::QUITTING;
			g_scorefile.add_score(m_game->gamemode(), score);
			set_up_exit_animation();
			m_next_state = make_async<title_state>();
		},
	};
	for (usize i = 0; i < BUTTONS_REGULAR.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS_REGULAR.size() + 1) * 30 + (i + 2) * 60};
		const tweened_position animation{{500 + offset, y}, {500, y}, 0.5_s};
		m_ui.emplace<text_button_widget>(BUTTONS_REGULAR[i], animation, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
										 loc_text_callback{BUTTONS_REGULAR[i]}, font::LANGUAGE, 48, i == 0 ? unpause_scb : scb,
										 std::move(action_cbs[i]), sound::CONFIRM);
	}
}

void pause_state::set_up_limited_ui()
{
	constexpr float TITLE_Y{500.0f - (BUTTONS_SPECIAL.size() + 1) * 30};
	constexpr tweened_position TITLE_ANIMATION{{500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};
	const tag title_tag{std::holds_alternative<replay_game_data>(m_data) ? T_REPLAY_PAUSED : T_TEST_PAUSED};
	m_ui.emplace<label_widget>(title_tag, TITLE_ANIMATION, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{title_tag},
							   tr::sys::ttf_style::NORMAL, 64);

	const status_callback scb{
		[this] { return m_substate == substate::PAUSED || m_substate == substate::PAUSING; },
	};
	const status_callback unpause_scb{[this] { return m_substate == substate::PAUSED; }};
	std::array<action_callback, BUTTONS_SPECIAL.size()> action_cbs{
		[this] {
			m_elapsed = 0;
			m_substate = substate::UNPAUSING;
			m_end_mouse_pos = g_mouse_pos;
			set_up_exit_animation();
			m_next_state = make_async<game_state>(m_game, m_data, fade_in::NO);
		},
		[this] {
			m_elapsed = 0;
			m_substate = substate::RESTARTING;
			set_up_exit_animation();
			if (std::holds_alternative<replay_game_data>(m_data)) {
				m_next_state = make_game_state_async<replay_game>(m_data, (replay_game&)*m_game);
			}
			else {
				m_next_state = make_game_state_async<active_game>(m_data, m_game->gamemode());
			}
		},
		[this] {
			m_elapsed = 0;
			m_substate = substate::QUITTING;
			set_up_exit_animation();
			if (std::holds_alternative<replay_game_data>(m_data)) {
				m_next_state = make_async<replays_state>();
			}
			else {
				m_next_state = make_async<gamemode_editor_state>(tr::get<test_game_data>(m_data).editor_data, m_game->gamemode());
			}
		},
	};
	for (usize i = 0; i < BUTTONS_SPECIAL.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS_SPECIAL.size() + 1) * 30 + (i + 2) * 60};
		const tweened_position animation{{500 + offset, y}, {500, y}, 0.5_s};
		m_ui.emplace<text_button_widget>(BUTTONS_SPECIAL[i], animation, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
										 loc_text_callback{BUTTONS_SPECIAL[i]}, font::LANGUAGE, 48, i == 0 ? unpause_scb : scb,
										 std::move(action_cbs[i]), sound::CONFIRM);
	}
}

void pause_state::set_up_exit_animation()
{
	if (std::holds_alternative<regular_game_data>(m_data)) {
		m_ui[T_PAUSED].move_y_and_hide(400 - (BUTTONS_REGULAR.size() + 1) * 30, 0.5_s);
		for (usize i = 0; i < BUTTONS_REGULAR.size(); ++i) {
			const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
			m_ui[BUTTONS_REGULAR[i]].move_x_and_hide(500 + offset, 0.5_s);
		}
	}
	else {
		const tag title_tag{std::holds_alternative<replay_game_data>(m_data) ? T_REPLAY_PAUSED : T_TEST_PAUSED};
		m_ui[title_tag].move_y_and_hide(400 - (BUTTONS_SPECIAL.size() + 1) * 30, 0.5_s);
		for (usize i = 0; i < BUTTONS_SPECIAL.size(); ++i) {
			const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
			m_ui[BUTTONS_SPECIAL[i]].move_x_and_hide(500 + offset, 0.5_s);
		}
	}
}