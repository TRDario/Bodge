#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"gamemode_manager"};
constexpr tag T_NEW_GAMEMODE{"new_gamemode"};
constexpr tag T_CLONE_GAMEMODE{"clone_gamemode"};
constexpr tag T_EDIT_GAMEMODE{"edit_gamemode"};
constexpr tag T_DELETE_GAMEMODES{"delete_gamemodes"};
constexpr tag T_EXIT{"exit"};

// Buttons in the center of the screen.
constexpr std::array<label_info, 4> CENTER_BUTTONS{{
	{T_NEW_GAMEMODE, "new_gamemode_tt"},
	{T_CLONE_GAMEMODE, "clone_gamemode_tt"},
	{T_EDIT_GAMEMODE, "edit_gamemode_tt"},
	{T_DELETE_GAMEMODES, "delete_gamemodes_tt"},
}};

constexpr selection_tree SELECTION_TREE{
    selection_tree_row{T_NEW_GAMEMODE},
    selection_tree_row{T_CLONE_GAMEMODE},
    selection_tree_row{T_EDIT_GAMEMODE},
    selection_tree_row{T_DELETE_GAMEMODES},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
    {"N"_kc, T_NEW_GAMEMODE},    {"1"_kc, T_NEW_GAMEMODE},
    {"C"_kc, T_CLONE_GAMEMODE},  {"2"_kc, T_CLONE_GAMEMODE},
    {"E"_kc, T_EDIT_GAMEMODE},   {"3"_kc, T_EDIT_GAMEMODE},
    {"D"_kc, T_DELETE_GAMEMODES}, {"4"_kc, T_DELETE_GAMEMODES},
	{"Escape"_kc, T_EXIT},       {"5"_kc, T_EXIT},
};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweened_position EXIT_ANIMATION{BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on
////////////////////////////////////////////////////////// GAMEMODE MANAGER STATE /////////////////////////////////////////////////////////

gamemode_manager_state::gamemode_manager_state(std::shared_ptr<playerless_game> game, animate_title animate_title)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_GAMEMODE_MANAGER}
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_GAMEMODE_MANAGER; },
	};

	// ACTION CALLBACKS

	const std::array<action_callback, CENTER_BUTTONS.size()> center_acbs{
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			set_up_exit_animation(animate_title::NO);
			m_next_state =
				make_async<gamemode_editor_state>(m_game, new_gamemode_editor_data{}, g_new_gamemode_draft, animate_subtitle::YES);
		},
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			set_up_exit_animation(animate_title::NO);
			m_next_state = make_async<gamemode_selector_state>(m_game, gamemode_selector_type::CLONE, animate_subtitle::YES);
		},
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			set_up_exit_animation(animate_title::NO);
			m_next_state = make_async<gamemode_selector_state>(m_game, gamemode_selector_type::EDIT, animate_subtitle::YES);
		},
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			set_up_exit_animation(animate_title::NO);
			m_next_state = make_async<gamemode_selector_state>(m_game, gamemode_selector_type::DELETE, animate_subtitle::YES);
		},
	};
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
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
	}

	for (usize i = 0; i < CENTER_BUTTONS.size(); ++i) {
		const float y{500.0f - ((CENTER_BUTTONS.size() - 1) * 50.0f) + i * 100};
		const tweened_position move_in{glm::vec2{i % 2 == 0 ? 600 : 400, y}, glm::vec2{500, y}, 0.5_s};
		m_ui.emplace<text_button_widget>(CENTER_BUTTONS[i].tag, move_in, tr::align::CENTER, 0.5_s,
										 loc_text_callback{CENTER_BUTTONS[i].tooltip}, loc_text_callback{CENTER_BUTTONS[i].tag},
										 font::LANGUAGE, 64, scb, center_acbs[i], sound::CONFIRM);
	}
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_ANIMATION, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

tr::next_state gamemode_manager_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_GAMEMODE_MANAGER:
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

//

void gamemode_manager_state::set_up_exit_animation(animate_title animate_title)
{
	if (bool(animate_title)) {
		m_ui[T_TITLE].move_and_hide(TOP_START_POS, 0.5_s);
	}
	for (usize i = 0; i < CENTER_BUTTONS.size(); ++i) {
		m_ui[CENTER_BUTTONS[i].tag].move_x_and_hide(i % 2 == 0 ? 400 : 600, 0.5_s);
	}
	m_ui[T_EXIT].move_and_hide(BOTTOM_START_POS, 0.5_s);
}