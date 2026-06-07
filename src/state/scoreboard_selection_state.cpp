///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements scoreboard_selection_state from state.hpp.                                                                                 //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"scoreboards"};
constexpr tag T_PLAYER_INFO{"player_info"};
constexpr tag T_VIEW_TIMES{"view_times"};
constexpr tag T_VIEW_SCORES{"view_scores"};
constexpr tag T_EXIT{"exit"};

// Selection tree used for the scoreboard selection menu.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_VIEW_TIMES},
	selection_tree_row{T_VIEW_SCORES},
	selection_tree_row{T_EXIT},
};

// Shorcut table used for the scoreboard selection menu.
constexpr shortcut_table SHORTCUTS{
	{"1"_kc, T_VIEW_TIMES},
	{"2"_kc, T_VIEW_SCORES},
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT},
};

// clang-format on
//////////////////////////////////////////////////////// SCOREBOARD SELECTION STATE ///////////////////////////////////////////////////////

scoreboard_selection_state::scoreboard_selection_state(std::shared_ptr<playerless_game> game, savefile savefile,
													   animate_title animate_title)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_SCOREBOARD_SELECTION}
	, m_savefile{std::move(savefile)}
{
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = bool(animate_title) ? tweened_position{TOP_START_POS, TITLE_POS, 0.5_s} : tweened_position{TITLE_POS},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = bool(animate_title) ? 0.5_s : 0_s,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_PLAYER_INFO, {
		.animation = bool(animate_title) ? tweened_position{TOP_START_POS, {500, 64}, 0.5_s} : tweened_position{{500, 64}},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = bool(animate_title) ? 0.5_s : 0_s,
		.text = constant_text{m_savefile.format_info()},
		.font_size = 32
	});
	m_ui.emplace<text_button_widget>(T_EXIT, {
		.animation = bool(animate_title) ? tweened_position{BOTTOM_START_POS, {500, 1000}, 0.5_s} : tweened_position{{500, 1000}},
		.alignment = tr::align::BOTTOM_CENTER,
		.unhide_time = bool(animate_title) ? 0.5_s : 0_s,
		.text = localized_text{T_EXIT},
		.status = [this] { return m_substate == substate::IN_SCOREBOARD_SELECTION; },
		.action = [this] { on_exit(); },
		.action_sound = sound::CANCEL
	});
	m_ui.emplace<text_button_widget>(T_VIEW_TIMES, {
		.animation = {{400, 450}, {500, 450}, 0.5_s},
		.text = localized_text{T_VIEW_TIMES},
		.font_size = 64,
		.status = [this] { return m_substate == substate::IN_SCOREBOARD_SELECTION; },
		.action = [this] { on_view_times(); }
	});
	m_ui.emplace<text_button_widget>(T_VIEW_SCORES, {
		.animation = {{600, 550}, {500, 550}, 0.5_s},
		.text = localized_text{T_VIEW_SCORES},
		.font_size = 64,
		.status = [this] { return m_substate == substate::IN_SCOREBOARD_SELECTION; },
		.action = [this] { on_view_scores(); }
	});
	// clang-format on
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

//

void scoreboard_selection_state::on_view_times()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_title::NO);
	m_next_state = make_async<scoreboard_state>(m_game, m_savefile, scoreboard::TIME);
}

void scoreboard_selection_state::on_view_scores()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_title::NO);
	m_next_state = make_async<scoreboard_state>(m_game, m_savefile, scoreboard::SCORE);
}

void scoreboard_selection_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_title::YES);
	m_next_state = make_async<title_state>(m_game);
}