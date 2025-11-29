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
	{"1"_kc, T_VIEW_TIMES},
	{"2"_kc, T_VIEW_SCORES},
	{"Enter"_kc, T_EXIT},
};

//

scoreboard_selection_state::scoreboard_selection_state(std::shared_ptr<playerless_game> game, bool returning_from_submenu)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{returning_from_submenu ? substate::RETURNING_FROM_SUBMENU : substate::IN_SCOREBOARD_SELECTION}
{
}

//

void scoreboard_selection_state::set_up_ui()
{
	using enum tr::align;

	// TEXT CALLBACKS

	const text_callback player_info_tcb{
		string_text{TR_FMT::format("{} {}: {}", g_loc["total_playtime"], g_scorefile.name, format_playtime(g_scorefile.playtime))},
	};

	//

	if (m_substate == substate::RETURNING_FROM_SUBMENU) {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, TOP_CENTER, 0, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
		m_ui.emplace<label_widget>(T_PLAYER_INFO, glm::vec2{500, 64}, TOP_CENTER, 0, NO_TOOLTIP, player_info_tcb, text_style::NORMAL, 32);
		m_ui.emplace<text_button_widget>(T_EXIT, glm::vec2{500, 1000}, BOTTOM_CENTER, 0, NO_TOOLTIP, tag_loc{T_EXIT}, font::LANGUAGE, 48,
										 interactible, on_exit, sound::CANCEL);
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
		m_ui.emplace<label_widget>(T_PLAYER_INFO, PLAYER_INFO_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, player_info_tcb, text_style::NORMAL,
								   32);
		m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_EXIT}, font::LANGUAGE, 48,
										 interactible, on_exit, sound::CANCEL);
	}
	m_ui.emplace<text_button_widget>(T_VIEW_TIMES, VIEW_TIMES_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_VIEW_TIMES}, font::LANGUAGE, 64,
									 interactible, on_view_times, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_VIEW_SCORES, VIEW_SCORES_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_VIEW_SCORES}, font::LANGUAGE,
									 64, interactible, on_view_scores, sound::CONFIRM);
}

next_state scoreboard_selection_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::RETURNING_FROM_SUBMENU:
		m_substate = substate::IN_SCOREBOARD_SELECTION;
		return tr::KEEP_STATE;
	case substate::IN_SCOREBOARD_SELECTION:
		return tr::KEEP_STATE;
	case substate::ENTERING_SUBMENU_OR_TITLE:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
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

//

bool scoreboard_selection_state::interactible()
{
	const scoreboard_selection_state& self{g_state_machine.get<scoreboard_selection_state>()};

	return self.m_substate != substate::ENTERING_SUBMENU_OR_TITLE;
}

void scoreboard_selection_state::on_view_times()
{
	scoreboard_selection_state& self{g_state_machine.get<scoreboard_selection_state>()};

	self.m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
	self.m_timer = 0;
	self.set_up_subscreen_animation();
	prepare_next_state<scoreboard_state>(self.m_game, scoreboard::TIME);
}

void scoreboard_selection_state::on_view_scores()
{
	scoreboard_selection_state& self{g_state_machine.get<scoreboard_selection_state>()};

	self.m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
	self.m_timer = 0;
	self.set_up_subscreen_animation();
	prepare_next_state<scoreboard_state>(self.m_game, scoreboard::SCORE);
}

void scoreboard_selection_state::on_exit()
{
	scoreboard_selection_state& self{g_state_machine.get<scoreboard_selection_state>()};

	self.m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<title_state>(self.m_game);
}