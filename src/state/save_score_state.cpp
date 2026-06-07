///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements save_score_state from state.hpp.                                                                                           //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
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

// Selection tree for the save score screen.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_INPUT},
	selection_tree_row{T_SAVE},
	selection_tree_row{T_CANCEL},
};

// Shortcut table for the save score screen.
constexpr shortcut_table SHORTCUTS{
	{"Enter"_kc, T_SAVE}, {"Ctrl+S"_kc, T_SAVE}, {"1"_kc, T_SAVE},
	{"Escape"_kc, T_CANCEL}, {"Q"_kc, T_CANCEL}, {"2"_kc, T_CANCEL},
};

// clang-format on
//////////////////////////////////////////////////////////// SAVE SCORE STATE /////////////////////////////////////////////////////////////

save_score_state::save_score_state(std::shared_ptr<game> game, savefile savefile, glm::vec2 mouse_pos, save_screen_flags flags)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), std::move(savefile), update_game::NO}
	, m_substate{substate_base::SAVING_SCORE | flags}
	, m_start_mouse_pos{mouse_pos}
	, m_score{
		  {},
		  current_timestamp(),
		  m_game->final_score(),
		  m_game->final_time(),
		  {!m_game->game_over(), debug_settings::instance().modified_game_speed()},
	  }
{
	set_up_ui();
}

save_score_state::save_score_state(std::shared_ptr<game> game, savefile savefile, save_screen_flags flags)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), std::move(savefile), update_game::YES}
	, m_substate{substate_base::SAVING_SCORE | (flags | save_screen_flags::GAME_OVER)}
	, m_score{
		  {},
		  current_timestamp(),
		  m_game->final_score(),
		  m_game->final_time(),
		  {!m_game->game_over(), debug_settings::instance().modified_game_speed()},
	  }
{
	set_up_ui();
}

//

tr::next_state save_score_state::tick()
{
	game_menu_state::tick();
	m_score.description = m_ui.as<multiline_input_widget<255>>(T_INPUT).contents();
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
	const float label_h{296 - renderer::instance().text_engine.line_skip(font::LANGUAGE, 32)};

	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = {TOP_START_POS, TITLE_POS, 0.5_s},
		.alignment = tr::align::TOP_CENTER,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_RESULTS, {
		.animation = {{500, 100}, {500, 200}, 0.5_s},
		.text = localized_text{T_RESULTS},
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_TIME_LABEL, {
		.animation = {{225, label_h}, {325, label_h}, 0.5_s},
		.text = localized_text{T_TIME_LABEL},
		.font_size = 32,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_TIME, {
		.animation = {{225, 300}, {325, 300}, 0.5_s},
		.text = constant_text{format_time(m_score.time)},
		.font_size = 64,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_SCORE_LABEL, {
		.animation = {{775, label_h}, {675, label_h}, 0.5_s},
		.text = localized_text{T_SCORE_LABEL},
		.font_size = 32,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_SCORE, {
		.animation = {{775, 300}, {675, 300}, 0.5_s},
		.text = constant_text{format_score(m_score.score)},
		.font_size = 64,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_DESCRIPTION, {
		.animation = {{600, 440}, {500, 440}, 0.5_s},
		.text = localized_text{T_DESCRIPTION}
	});
	m_ui.emplace<multiline_input_widget<255>>(T_INPUT, {
		.animation = {{600, 475}, {500, 475}, 0.5_s},
		.alignment = tr::align::TOP_CENTER,
		.width = 800,
		.max_lines = 10,
		.font_size = 24,
		.status = [this] { return to_base(m_substate) == substate_base::SAVING_SCORE; }
	});
	m_ui.emplace<text_button_widget>(T_SAVE, {
		.animation = {BOTTOM_START_POS, {500, 950}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.text = localized_text{T_SAVE},
		.status = [this] { return to_base(m_substate) == substate_base::SAVING_SCORE; },
		.action = [this] { on_save(); }
	});
	m_ui.emplace<text_button_widget>(T_CANCEL, {
		.animation = {BOTTOM_START_POS, {500, 1000}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.text = localized_text{T_CANCEL},
		.status = [this] { return to_base(m_substate) == substate_base::SAVING_SCORE; },
		.action = [this] { on_cancel(); },
		.action_sound = sound::CANCEL
	});
	// clang-format on
}

void save_score_state::set_up_exit_animation()
{
	m_ui[T_TITLE].move_and_hide(TOP_START_POS, 0.5_s);
	m_ui[T_RESULTS].move_y_and_hide(100, 0.5_s);
	m_ui[T_TIME_LABEL].move_x_and_hide(225, 0.5_s);
	m_ui[T_TIME].move_x_and_hide(225, 0.5_s);
	m_ui[T_SCORE_LABEL].move_x_and_hide(775, 0.5_s);
	m_ui[T_SCORE].move_x_and_hide(775, 0.5_s);
	m_ui[T_DESCRIPTION].move_x_and_hide(400, 0.5_s);
	m_ui[T_INPUT].move_x_and_hide(400, 0.5_s);
	m_ui[T_SAVE].move_and_hide(BOTTOM_START_POS, 0.5_s);
	m_ui[T_CANCEL].move_and_hide(BOTTOM_START_POS, 0.5_s);
}

//

void save_score_state::on_save()
{
	m_substate = substate_base::RETURNING_OR_ENTERING_SAVE_REPLAY | to_flags(m_substate);
	m_elapsed = 0;
	set_up_exit_animation();
	m_savefile.add_score(m_game->gamemode(), m_score);
	m_savefile.save_to_file();
	m_next_state = make_async<save_replay_state>(m_game, m_savefile, to_flags(m_substate));
}

void save_score_state::on_cancel()
{
	m_substate = substate_base::RETURNING_OR_ENTERING_SAVE_REPLAY | to_flags(m_substate);
	m_elapsed = 0;
	set_up_exit_animation();
	if (to_flags(m_substate) & save_screen_flags::GAME_OVER) {
		m_next_state = make_async<game_over_state>(m_game, m_savefile, blur_in::NO);
	}
	else {
		m_next_state = make_async<pause_state>(m_game, m_savefile, regular_game_data{}, m_start_mouse_pos, blur_in::NO);
	}
}