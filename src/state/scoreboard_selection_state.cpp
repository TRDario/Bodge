#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"scoreboards"};
constexpr tag T_PLAYER_INFO{"player_info"};
constexpr tag T_VIEW_TIMES{"view_times"};
constexpr tag T_VIEW_SCORES{"view_scores"};
constexpr tag T_EXIT{"exit"};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_VIEW_TIMES},
	selection_tree_row{T_VIEW_SCORES},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{"1"_kc, T_VIEW_TIMES},
	{"2"_kc, T_VIEW_SCORES},
	{"Escape"_kc, T_EXIT},
};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweened_position VIEW_TIMES_MOVE_IN{{400, 450}, {500, 450}, 0.5_s};
constexpr tweened_position VIEW_SCORES_MOVE_IN{{600, 550}, {500, 550}, 0.5_s};
constexpr tweened_position PLAYER_INFO_MOVE_IN{TOP_START_POS, {500, 64}, 0.5_s};
constexpr tweened_position EXIT_MOVE_IN{BOTTOM_START_POS, {500, 1000}, 0.5_s};

//////////////////////////////////////////////////////// SCOREBOARD SELECTION STATE ///////////////////////////////////////////////////////

scoreboard_selection_state::scoreboard_selection_state(std::shared_ptr<playerless_game> game, animate_title animate_title)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_SCOREBOARD_SELECTION}
{
	// STATUS CALLBACKS

	const status_callback scb{[this] { return m_substate == substate::IN_SCOREBOARD_SELECTION; }};

	// ACTION CALLBACKS

	const action_callback view_times_acb{[this] {
		m_substate = substate::EXITING;
		m_elapsed = 0;
		set_up_exit_animation(animate_title::NO);
		m_next_state = make_async<scoreboard_state>(m_game, scoreboard::TIME);
	}};
	const action_callback view_scores_acb{[this] {
		m_substate = substate::EXITING;
		m_elapsed = 0;
		set_up_exit_animation(animate_title::NO);
		m_next_state = make_async<scoreboard_state>(m_game, scoreboard::SCORE);
	}};
	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING;
		m_elapsed = 0;
		set_up_exit_animation(animate_title::YES);
		m_next_state = make_async<title_state>(m_game);
	}};

	//

	if (bool(animate_title)) {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
		m_ui.emplace<label_widget>(T_PLAYER_INFO, PLAYER_INFO_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP,
								   const_text_callback{g_scorefile.format_player_info()}, tr::sys::ttf_style::NORMAL, 32);
		m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
										 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
		m_ui.emplace<label_widget>(T_PLAYER_INFO, glm::vec2{500, 64}, tr::align::TOP_CENTER, 0, NO_TOOLTIP,
								   const_text_callback{g_scorefile.format_player_info()}, tr::sys::ttf_style::NORMAL, 32);
		m_ui.emplace<text_button_widget>(T_EXIT, glm::vec2{500, 1000}, tr::align::BOTTOM_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_EXIT},
										 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	}
	m_ui.emplace<text_button_widget>(T_VIEW_TIMES, VIEW_TIMES_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 loc_text_callback{T_VIEW_TIMES}, font::LANGUAGE, 64, scb, view_times_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_VIEW_SCORES, VIEW_SCORES_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 loc_text_callback{T_VIEW_SCORES}, font::LANGUAGE, 64, scb, view_scores_acb, sound::CONFIRM);
}

tr::next_state scoreboard_selection_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_SCOREBOARD_SELECTION:
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

//

void scoreboard_selection_state::set_up_exit_animation(animate_title animate_title)
{
	if (bool(animate_title)) {
		m_ui[T_TITLE].move_and_hide(TOP_START_POS, 0.5_s);
		m_ui[T_PLAYER_INFO].move_and_hide(TOP_START_POS, 0.5_s);
		m_ui[T_EXIT].move_and_hide(BOTTOM_START_POS, 0.5_s);
	}
	m_ui[T_VIEW_TIMES].move_x_and_hide(600, 0.5_s);
	m_ui[T_VIEW_SCORES].move_x_and_hide(400, 0.5_s);
}