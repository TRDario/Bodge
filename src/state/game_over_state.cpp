#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"game_over"};
constexpr tag T_TIME_LABEL{"time"};
constexpr tag T_TIME{"time_display"};
constexpr tag T_BEST_TIME{"best_time"};
constexpr tag T_SCORE_LABEL{"score"};
constexpr tag T_SCORE{"score_display"};
constexpr tag T_BEST_SCORE{"best_score"};
constexpr tag T_SAVE_AND_RESTART{"save_and_restart"};
constexpr tag T_RESTART{"restart"};
constexpr tag T_SAVE_AND_QUIT{"save_and_quit"};
constexpr tag T_QUIT{"quit"};

// Button list.
constexpr std::array<tag, 4> BUTTONS{T_SAVE_AND_RESTART, T_RESTART, T_SAVE_AND_QUIT, T_QUIT};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_SAVE_AND_RESTART},
	selection_tree_row{T_RESTART},
	selection_tree_row{T_SAVE_AND_QUIT},
	selection_tree_row{T_QUIT},
};

constexpr shortcut_table SHORTCUTS{
	{"Shift+R"_kc, T_SAVE_AND_RESTART},
	{"1"_kc, T_SAVE_AND_RESTART},
	{"R"_kc, T_RESTART},
	{"2"_kc, T_RESTART},
	{"Shift+Escape"_kc, T_SAVE_AND_QUIT},
	{"Shift+Q"_kc, T_SAVE_AND_QUIT},
	{"Shift+E"_kc, T_SAVE_AND_QUIT},
	{"3"_kc, T_SAVE_AND_QUIT},
	{"Escape"_kc, T_QUIT},
	{"Q"_kc, T_QUIT},
	{"4"_kc, T_QUIT},
};

constexpr float TITLE_Y{500.0f - (BUTTONS.size() + 3) * 30};

constexpr tweened_position TITLE_MOVE_IN{{500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};

///////////////////////////////////////////////////////////// GAME OVER STATE /////////////////////////////////////////////////////////////

game_over_state::game_over_state(std::shared_ptr<game> game, blur_in blur_in)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), update_game::YES}
	, m_substate{blur_in == blur_in::YES ? substate::BLURRING_IN : substate::GAME_OVER}
{
	// HEIGHTS AND MOVE-INS

	const float result_h{(500 - (BUTTONS.size() - 0.75f) * 30) + 4};
	const float label_h{result_h - g_text_engine.line_skip(font::LANGUAGE, 48) + 14};
	const float best_h{result_h + g_text_engine.line_skip(font::LANGUAGE, 48) - 14};

	const tweened_position time_label_move_in{{175, label_h}, {275, label_h}, 0.5_s};
	const tweened_position time_move_in{{175, result_h}, {275, result_h}, 0.5_s};
	const tweened_position best_time_move_in{{175, best_h}, {275, best_h}, 0.5_s};
	const tweened_position score_label_move_in{{825, label_h}, {725, label_h}, 0.5_s};
	const tweened_position score_move_in{{825, result_h}, {725, result_h}, 0.5_s};
	const tweened_position best_score_move_in{{825, best_h}, {725, best_h}, 0.5_s};

	// STATUS CALLBACKS

	const status_callback scb{[this] { return m_substate == substate::BLURRING_IN || m_substate == substate::GAME_OVER; }};

	// ACTION CALLBACKS

	std::array<action_callback, BUTTONS.size()> action_cbs{
		[this] {
			m_elapsed = 0;
			m_substate = substate::SAVING;
			set_up_exit_animation();
			m_next_state = make_async<save_score_state>(m_game, save_screen_flags::RESTARTING);
		},
		[this] {
			const score_flags score_flags{false, g_cli_settings.game_speed != 1};
			const score_entry score{{}, current_timestamp(), m_game->final_score(), m_game->final_time(), score_flags};

			m_elapsed = 0;
			m_substate = substate::RESTARTING;
			g_scorefile.add_score(m_game->gamemode(), score);
			set_up_exit_animation();
			m_next_state = make_game_state_async<active_game>(game_type::REGULAR, fade_in::YES, m_game->gamemode());
		},
		[this] {
			m_elapsed = 0;
			m_substate = substate::SAVING;
			set_up_exit_animation();
			m_next_state = make_async<save_score_state>(m_game, save_screen_flags::NONE);
		},
		[this] {
			const score_flags score_flags{false, g_cli_settings.game_speed != 1};
			const score_entry score{{}, current_timestamp(), m_game->final_score(), m_game->final_time(), score_flags};

			m_elapsed = 0;
			m_substate = substate::QUITTING;
			g_scorefile.add_score(m_game->gamemode(), score);
			set_up_exit_animation();
			m_next_state = make_async<title_state>();
		},
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_TIME_LABEL, time_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TIME_LABEL},
							   tr::sys::ttf_style::NORMAL, 24, YELLOW);
	m_ui.emplace<label_widget>(T_TIME, time_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{format_time(m_game->final_time())}, tr::sys::ttf_style::NORMAL, 64, YELLOW);
	m_ui.emplace<label_widget>(T_BEST_TIME, best_time_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, best_time_text_callback(),
							   tr::sys::ttf_style::NORMAL, 24, YELLOW);
	m_ui.emplace<label_widget>(T_SCORE_LABEL, score_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SCORE_LABEL},
							   tr::sys::ttf_style::NORMAL, 24, YELLOW);
	m_ui.emplace<label_widget>(T_SCORE, score_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{format_score(m_game->final_score())}, tr::sys::ttf_style::NORMAL, 64, YELLOW);
	m_ui.emplace<label_widget>(T_BEST_SCORE, best_score_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, best_score_text_callback(),
							   tr::sys::ttf_style::NORMAL, 24, YELLOW);
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS.size() + 3) * 30 + (i + 4) * 60};
		const tweened_position pos{{500 + offset, y}, {500, y}, 0.5_s};
		m_ui.emplace<text_button_widget>(BUTTONS[i], pos, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{BUTTONS[i]},
										 font::LANGUAGE, 48, scb, std::move(action_cbs[i]), sound::CONFIRM);
	}
}

//

tr::next_state game_over_state::tick()
{
	game_menu_state::tick();
	switch (m_substate) {
	case substate::BLURRING_IN:
		if (m_elapsed >= 0.5_s) {
			m_elapsed = 0;
			m_substate = substate::GAME_OVER;
		}
		[[fallthrough]];
	case substate::GAME_OVER:
		if (g_scorefile.bests(m_game->gamemode()).time < m_game->final_time()) {
			if (m_elapsed % 0.5_s == 0) {
				m_ui[T_BEST_TIME].hide();
			}
			else if (m_elapsed % 0.5_s == 0.25_s) {
				m_ui[T_BEST_TIME].unhide();
			}
		}
		if (g_scorefile.bests(m_game->gamemode()).score < m_game->final_score()) {
			if (m_elapsed % 0.5_s == 0) {
				m_ui[T_BEST_SCORE].hide();
			}
			else if (m_elapsed % 0.5_s == 0.25_s) {
				m_ui[T_BEST_SCORE].unhide();
			}
		}
		return tr::KEEP_STATE;
	case substate::SAVING:
	case substate::RESTARTING:
		return next_state_if_after(0.5_s);
	case substate::QUITTING:
		if (m_elapsed >= 0.5_s) {
			g_audio.play_song("menu", 1.0s);
			return m_next_state.get();
		}
		else {
			return tr::KEEP_STATE;
		}
	}
}

//

float game_over_state::fade_overlay_opacity()
{
	return m_substate == substate::RESTARTING || m_substate == substate::QUITTING ? m_elapsed / 0.5_sf : 0;
}

float game_over_state::saturation_factor()
{
	switch (m_substate) {
	case substate::GAME_OVER:
	case substate::SAVING:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 0.35f;
	case substate::BLURRING_IN:
		return 1 - m_elapsed / 0.5_sf * 0.65f;
	}
}

float game_over_state::blur_strength()
{
	switch (m_substate) {
	case substate::GAME_OVER:
	case substate::SAVING:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 10;
	case substate::BLURRING_IN:
		return m_elapsed / 0.5_sf * 10;
	}
}

//

text_callback game_over_state::best_time_text_callback() const
{
	const bests& bests{g_scorefile.bests(m_game->gamemode())};

	if (bests.time < m_game->final_time()) {
		return loc_text_callback{"new_personal_best"};
	}
	else {
		return const_text_callback{TR_FMT::format("{}: {}", g_loc["personal_best"], format_time(bests.time))};
	}
}

text_callback game_over_state::best_score_text_callback() const
{
	const bests& bests{g_scorefile.bests(m_game->gamemode())};

	if (bests.score < m_game->final_score()) {
		return loc_text_callback{"new_personal_best"};
	}
	else {
		return const_text_callback{TR_FMT::format("{}: {}", g_loc["personal_best"], bests.score)};
	}
}

//

void game_over_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.move_y(TITLE_Y - 100, 0.5_s);
	m_ui[T_TIME_LABEL].pos.move_x(150, 0.5_s);
	m_ui[T_TIME].pos.move_x(150, 0.5_s);
	m_ui[T_BEST_TIME].pos.move_x(150, 0.5_s);
	m_ui[T_SCORE_LABEL].pos.move_x(850, 0.5_s);
	m_ui[T_SCORE].pos.move_x(850, 0.5_s);
	m_ui[T_BEST_SCORE].pos.move_x(850, 0.5_s);
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		m_ui[BUTTONS[i]].pos.move_x(500 + (i % 2 != 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f), 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
}