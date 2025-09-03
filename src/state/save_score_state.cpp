#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

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
	{{tr::system::keycode::ENTER}, T_SAVE},
	{{tr::system::keycode::S}, T_SAVE},
	{{tr::system::keycode::TOP_ROW_1}, T_SAVE},
	{{tr::system::keycode::ESCAPE}, T_CANCEL},
	{{tr::system::keycode::C}, T_CANCEL},
	{{tr::system::keycode::TOP_ROW_2}, T_CANCEL},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> RESULTS_MOVE_IN{tween::CUBIC, {500, 100}, {500, 200}, 0.5_s};
constexpr tweener<glm::vec2> TIME_MOVE_IN{tween::CUBIC, {225, 300}, {325, 300}, 0.5_s};
constexpr tweener<glm::vec2> SCORE_MOVE_IN{tween::CUBIC, {775, 300}, {675, 300}, 0.5_s};
constexpr tweener<glm::vec2> DESCRIPTION_MOVE_IN{tween::CUBIC, {600, 440}, {500, 440}, 0.5_s};
constexpr tweener<glm::vec2> DESCRIPTION_INPUT_MOVE_IN{tween::CUBIC, {600, 475}, {500, 475}, 0.5_s};
constexpr tweener<glm::vec2> SAVE_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweener<glm::vec2> CANCEL_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

save_score_state::save_score_state(std::unique_ptr<game>&& game, glm::vec2 mouse_pos, save_screen_flags flags)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), false}
	, m_substate{substate_base::SAVING_SCORE | flags}
	, m_start_mouse_pos{mouse_pos}
	, m_score{
		  {},
		  current_timestamp(),
		  m_game->final_score(),
		  m_game->final_time(),
		  {!m_game->game_over(), engine::cli_settings.game_speed != 1.0f},
	  }
{
	set_up_ui();
}

save_score_state::save_score_state(std::unique_ptr<game>&& game, save_screen_flags flags)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), true}
	, m_substate{substate_base::SAVING_SCORE | (flags | save_screen_flags::GAME_OVER)}
	, m_score{
		  {},
		  current_timestamp(),
		  m_game->final_score(),
		  m_game->final_time(),
		  {!m_game->game_over(), engine::cli_settings.game_speed != 1.0f},
	  }
{
	set_up_ui();
}

//

std::unique_ptr<tr::state> save_score_state::update(tr::duration)
{
	game_menu_state::update({});
	m_score.description = m_ui.as<multiline_input_widget<255>>(T_INPUT).buffer;
	switch (to_base(m_substate)) {
	case substate_base::SAVING_SCORE:
		return nullptr;
	case substate_base::RETURNING:
		if (m_timer >= 0.5_s) {
			if (to_flags(m_substate) & save_screen_flags::GAME_OVER) {
				return std::make_unique<game_over_state>(std::move(m_game), false);
			}
			else {
				return std::make_unique<pause_state>(std::move(m_game), game_type::REGULAR, m_start_mouse_pos, false);
			}
		}
		else {
			return nullptr;
		}
	case substate_base::ENTERING_SAVE_REPLAY:
		return m_timer >= 0.5_s ? std::make_unique<save_replay_state>(std::move(m_game), to_flags(m_substate)) : nullptr;
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

	const float label_h{296 - engine::line_skip(font::LANGUAGE, 32)};
	const tweener<glm::vec2> best_time_label_move_in{tween::CUBIC, {225, label_h}, {325, label_h}, 0.5_s};
	const tweener<glm::vec2> best_score_label_move_in{tween::CUBIC, {775, label_h}, {675, label_h}, 0.5_s};

	// TEXT CALLBACKS

	text_callback time_tcb{string_text_callback{timer_text(m_score.time)}};
	text_callback score_tcb{string_text_callback{TR_FMT::format("{:05}", m_score.score)}};

	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return to_base(m_substate) == substate_base::SAVING_SCORE; },
	};

	// ACTION CALLBACKS

	const action_callback save_acb{
		[this] {
			m_substate = substate_base::ENTERING_SAVE_REPLAY | to_flags(m_substate);
			m_timer = 0;
			set_up_exit_animation();
			engine::scorefile.add_score(m_game->gamemode(), m_score);
		},
	};
	const action_callback cancel_acb{
		[this] {
			m_substate = substate_base::RETURNING | to_flags(m_substate);
			m_timer = 0;
			set_up_exit_animation();
		},
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_RESULTS, RESULTS_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_RESULTS},
							   tr::system::ttf_style::NORMAL, 48, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_TIME_LABEL, best_time_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TIME_LABEL},
							   tr::system::ttf_style::NORMAL, 32, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_TIME, TIME_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(time_tcb),
							   tr::system::ttf_style::NORMAL, 64, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_SCORE_LABEL, best_score_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   loc_text_callback{T_SCORE_LABEL}, tr::system::ttf_style::NORMAL, 32, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_SCORE, SCORE_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(score_tcb),
							   tr::system::ttf_style::NORMAL, 64, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_DESCRIPTION},
							   tr::system::ttf_style::NORMAL, 48);
	m_ui.emplace<multiline_input_widget<255>>(T_INPUT, DESCRIPTION_INPUT_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, 800, 10, 24, scb);
	m_ui.emplace<text_button_widget>(T_SAVE, SAVE_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SAVE},
									 font::LANGUAGE, 48, scb, save_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_CANCEL, CANCEL_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_CANCEL},
									 font::LANGUAGE, 48, scb, cancel_acb, sound::CANCEL);
}

void save_score_state::set_up_exit_animation()
{
	widget& time_label{m_ui[T_TIME_LABEL]};
	widget& score_label{m_ui[T_SCORE_LABEL]};
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_RESULTS].pos.change(tween::CUBIC, {500, 100}, 0.5_s);
	time_label.pos.change(tween::CUBIC, {225, glm::vec2{time_label.pos}.y}, 0.5_s);
	m_ui[T_TIME].pos.change(tween::CUBIC, {225, 300}, 0.5_s);
	score_label.pos.change(tween::CUBIC, {775, glm::vec2{score_label.pos}.y}, 0.5_s);
	m_ui[T_SCORE].pos.change(tween::CUBIC, {775, 300}, 0.5_s);
	m_ui[T_DESCRIPTION].pos.change(tween::CUBIC, {400, 440}, 0.5_s);
	m_ui[T_INPUT].pos.change(tween::CUBIC, {400, 475}, 0.5_s);
	m_ui[T_SAVE].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui[T_CANCEL].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}