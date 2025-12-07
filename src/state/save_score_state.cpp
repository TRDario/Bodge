#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"save_score"};
constexpr tag T_RESULTS{"results"};
constexpr tag T_TIME_LABEL{"time"};
constexpr tag T_TIME{"time_display"};
constexpr tag T_SCORE_LABEL{"score"};
constexpr tag T_SCORE{"score_display"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_INPUT{"input"};
constexpr tag T_SAVE{"save"};
constexpr tag T_CANCEL{"cancel"};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_INPUT},
	selection_tree_row{T_SAVE},
	selection_tree_row{T_CANCEL},
};

constexpr shortcut_table SHORTCUTS{
	{"Enter"_kc, T_SAVE},
	{"S"_kc, T_SAVE},
	{"1"_kc, T_SAVE},
	{"Escape"_kc, T_CANCEL},
	{"C"_kc, T_CANCEL},
	{"2"_kc, T_CANCEL},
};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweened_position RESULTS_MOVE_IN{{500, 100}, {500, 200}, 0.5_s};
constexpr tweened_position TIME_MOVE_IN{{225, 300}, {325, 300}, 0.5_s};
constexpr tweened_position SCORE_MOVE_IN{{775, 300}, {675, 300}, 0.5_s};
constexpr tweened_position DESCRIPTION_MOVE_IN{{600, 440}, {500, 440}, 0.5_s};
constexpr tweened_position DESCRIPTION_INPUT_MOVE_IN{{600, 475}, {500, 475}, 0.5_s};
constexpr tweened_position SAVE_MOVE_IN{BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweened_position CANCEL_MOVE_IN{BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on
//////////////////////////////////////////////////////////// SAVE SCORE STATE /////////////////////////////////////////////////////////////

save_score_state::save_score_state(std::shared_ptr<game> game, glm::vec2 mouse_pos, save_screen_flags flags)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), update_game::NO}
	, m_substate{substate_base::SAVING_SCORE | flags}
	, m_start_mouse_pos{mouse_pos}
	, m_score{
		  {}, current_timestamp(), m_game->final_score(), m_game->final_time(), {!m_game->game_over(), g_cli_settings.game_speed != 1.0f},
	  }
{
	set_up_ui();
}

save_score_state::save_score_state(std::shared_ptr<game> game, save_screen_flags flags)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), update_game::YES}
	, m_substate{substate_base::SAVING_SCORE | (flags | save_screen_flags::GAME_OVER)}
	, m_score{
		  {}, current_timestamp(), m_game->final_score(), m_game->final_time(), {!m_game->game_over(), g_cli_settings.game_speed != 1.0f},
	  }
{
	set_up_ui();
}

//

tr::next_state save_score_state::tick()
{
	game_menu_state::tick();
	m_score.description = m_ui.as<multiline_input_widget<255>>(T_INPUT).buffer;
	switch (to_base(m_substate)) {
	case substate_base::SAVING_SCORE:
		return tr::KEEP_STATE;
	case substate_base::RETURNING_OR_ENTERING_SAVE_REPLAY:
		return next_state_if_after(0.5_s);
	}
}

//

save_score_state::substate operator|(const save_score_state::substate_base& l, const save_screen_flags& r)
{
	return save_score_state::substate(int(l) | int(r));
}

save_score_state::substate_base to_base(save_score_state::substate state)
{
	return save_score_state::substate_base(int(state) & 0x3);
}

save_screen_flags to_flags(save_score_state::substate state)
{
	return save_screen_flags(int(state) & int(save_screen_flags::MASK));
}

void save_score_state::set_up_ui()
{
	// MOVE-INS

	const float label_h{296 - g_text_engine.line_skip(font::LANGUAGE, 32)};
	const tweened_position best_time_label_move_in{{225, label_h}, {325, label_h}, 0.5_s};
	const tweened_position best_score_label_move_in{{775, label_h}, {675, label_h}, 0.5_s};

	// STATUS CALLBACKS

	const status_callback scb{[this] { return to_base(m_substate) == substate_base::SAVING_SCORE; }};

	// ACTION CALLBACKS

	const action_callback save_acb{[this] {
		m_substate = substate_base::RETURNING_OR_ENTERING_SAVE_REPLAY | to_flags(m_substate);
		m_elapsed = 0;
		set_up_exit_animation();
		g_scorefile.add_score(m_game->gamemode(), m_score);
		m_next_state = make_async<save_replay_state>(m_game, to_flags(m_substate));
	}};
	const action_callback cancel_acb{[this] {
		m_substate = substate_base::RETURNING_OR_ENTERING_SAVE_REPLAY | to_flags(m_substate);
		m_elapsed = 0;
		set_up_exit_animation();
		if (to_flags(m_substate) & save_screen_flags::GAME_OVER) {
			m_next_state = make_async<game_over_state>(m_game, blur_in::NO);
		}
		else {
			m_next_state = make_async<pause_state>(m_game, game_type::REGULAR, m_start_mouse_pos, blur_in::NO);
		}
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_RESULTS, RESULTS_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_RESULTS},
							   tr::sys::ttf_style::NORMAL, 48, YELLOW);
	m_ui.emplace<label_widget>(T_TIME_LABEL, best_time_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TIME_LABEL},
							   tr::sys::ttf_style::NORMAL, 32, YELLOW);
	m_ui.emplace<label_widget>(T_TIME, TIME_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, const_text_callback{format_time(m_score.time)},
							   tr::sys::ttf_style::NORMAL, 64, YELLOW);
	m_ui.emplace<label_widget>(T_SCORE_LABEL, best_score_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   loc_text_callback{T_SCORE_LABEL}, tr::sys::ttf_style::NORMAL, 32, YELLOW);
	m_ui.emplace<label_widget>(T_SCORE, SCORE_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{format_score(m_score.score)}, tr::sys::ttf_style::NORMAL, 64, YELLOW);
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_DESCRIPTION},
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<multiline_input_widget<255>>(T_INPUT, DESCRIPTION_INPUT_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, 800, 10, 24, scb);
	m_ui.emplace<text_button_widget>(T_SAVE, SAVE_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SAVE},
									 font::LANGUAGE, 48, scb, save_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_CANCEL, CANCEL_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_CANCEL},
									 font::LANGUAGE, 48, scb, cancel_acb, sound::CANCEL);
}

void save_score_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.move(TOP_START_POS, 0.5_s);
	m_ui[T_RESULTS].pos.move_y(100, 0.5_s);
	m_ui[T_TIME_LABEL].pos.move_x(225, 0.5_s);
	m_ui[T_TIME].pos.move_x(225, 0.5_s);
	m_ui[T_SCORE_LABEL].pos.move_x(775, 0.5_s);
	m_ui[T_SCORE].pos.move_x(775, 0.5_s);
	m_ui[T_DESCRIPTION].pos.move_x(400, 0.5_s);
	m_ui[T_INPUT].pos.move_x(400, 0.5_s);
	m_ui[T_SAVE].pos.move(BOTTOM_START_POS, 0.5_s);
	m_ui[T_CANCEL].pos.move(BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}