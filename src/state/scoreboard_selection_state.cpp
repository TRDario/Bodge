#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

//

constexpr tag T_TITLE{"scoreboards"};
constexpr tag T_PLAYER_INFO{"player_info"};
constexpr tag T_VIEW_TIMES{"view_times"};
constexpr tag T_VIEW_SCORES{"view_scores"};
constexpr tag T_EXIT{"exit"};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> VIEW_TIMES_MOVE_IN{tween::CUBIC, {400, 450}, {500, 450}, 0.5_s};
constexpr tweener<glm::vec2> VIEW_SCORES_MOVE_IN{tween::CUBIC, {600, 550}, {500, 550}, 0.5_s};
constexpr tweener<glm::vec2> PLAYER_INFO_MOVE_IN{tween::CUBIC, TOP_START_POS, {500, 64}, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_VIEW_TIMES},
	selection_tree_row{T_VIEW_SCORES},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::TOP_ROW_1}, T_VIEW_TIMES},
	{{tr::system::keycode::TOP_ROW_2}, T_VIEW_SCORES},
	{{tr::system::keycode::ESCAPE}, T_EXIT},
};

//

scoreboard_selection_state::scoreboard_selection_state(std::unique_ptr<playerless_game>&& game, bool returning_from_subscreen)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_SCOREBOARD_SELECTION}
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_SCOREBOARD_SELECTION; },
	};

	// ACTION CALLBACKS

	const action_callback view_times_acb{
		[this] {
			m_substate = substate::ENTERING_TIME_SCOREBOARD;
			m_timer = 0;
			set_up_subscreen_animation();
		},
	};
	const action_callback view_scores_acb{
		[this] {
			m_substate = substate::ENTERING_SCORE_SCOREBOARD;
			m_timer = 0;
			set_up_subscreen_animation();
		},
	};
	const action_callback exit_acb{
		[this] {
			m_substate = substate::EXITING_TO_TITLE;
			m_timer = 0;
			set_up_exit_animation();
		},
	};

	// TEXT CALLBACKS

	const text_callback player_info_tcb{string_text_callback{TR_FMT::format(
		"{} {}: {}:{:02}:{:02}", engine::loc["total_playtime"], engine::scorefile.name, engine::scorefile.playtime / (SECOND_TICKS * 3600),
		(engine::scorefile.playtime % (SECOND_TICKS * 3600)) / (SECOND_TICKS * 60),
		(engine::scorefile.playtime % (SECOND_TICKS * 60) / SECOND_TICKS))}};

	//

	if (returning_from_subscreen) {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::system::ttf_style::NORMAL, 64);
		m_ui.emplace<label_widget>(T_PLAYER_INFO, glm::vec2{500, 64}, tr::align::TOP_CENTER, 0, NO_TOOLTIP, player_info_tcb,
								   tr::system::ttf_style::NORMAL, 32);
		m_ui.emplace<text_button_widget>(T_EXIT, glm::vec2{500, 1000}, tr::align::BOTTOM_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_EXIT},
										 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::system::ttf_style::NORMAL, 64);
		m_ui.emplace<label_widget>(T_PLAYER_INFO, PLAYER_INFO_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, player_info_tcb,
								   tr::system::ttf_style::NORMAL, 32);
		m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
										 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	}
	m_ui.emplace<text_button_widget>(T_VIEW_TIMES, VIEW_TIMES_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 loc_text_callback{T_VIEW_TIMES}, font::LANGUAGE, 64, scb, view_times_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_VIEW_SCORES, VIEW_SCORES_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 loc_text_callback{T_VIEW_SCORES}, font::LANGUAGE, 64, scb, view_scores_acb, sound::CONFIRM);
}

std::unique_ptr<tr::state> scoreboard_selection_state::update(tr::duration)
{
	main_menu_state::update({});
	switch (m_substate) {
	case substate::IN_SCOREBOARD_SELECTION:
		return nullptr;
	case substate::ENTERING_TIME_SCOREBOARD:
		if (m_timer >= 0.5_s) {
			return std::make_unique<scoreboard_state>(release_game(), scoreboard::TIME);
		}
		return nullptr;
	case substate::ENTERING_SCORE_SCOREBOARD:
		if (m_timer >= 0.5_s) {
			return std::make_unique<scoreboard_state>(release_game(), scoreboard::SCORE);
		}
		return nullptr;
	case substate::EXITING_TO_TITLE:
		if (m_timer >= 0.5_s) {
			return std::make_unique<title_state>(release_game());
		}
		return nullptr;
	}
}

//

void scoreboard_selection_state::set_up_subscreen_animation()
{
	widget& view_times{m_ui[T_VIEW_TIMES]};
	widget& view_scores{m_ui[T_VIEW_SCORES]};
	view_times.pos.change(tween::CUBIC, {600, 450}, 0.5_s);
	view_scores.pos.change(tween::CUBIC, {400, 550}, 0.5_s);
	view_times.hide(0.5_s);
	view_scores.hide(0.5_s);
}

void scoreboard_selection_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_PLAYER_INFO].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_VIEW_TIMES].pos.change(tween::CUBIC, {600, 450}, 0.5_s);
	m_ui[T_VIEW_SCORES].pos.change(tween::CUBIC, {400, 550}, 0.5_s);
	m_ui[T_EXIT].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}