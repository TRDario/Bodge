///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements gamemode_manager_state from state.hpp.                                                                                     //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
constexpr std::array CENTER_BUTTONS{
	label_info{T_NEW_GAMEMODE, "new_gamemode_tt"},
	label_info{T_CLONE_GAMEMODE, "clone_gamemode_tt"},
	label_info{T_EDIT_GAMEMODE, "edit_gamemode_tt"},
	label_info{T_DELETE_GAMEMODES, "delete_gamemodes_tt"},
};

// Selection tree for the gamemode manager menu.
constexpr selection_tree SELECTION_TREE{
    selection_tree_row{T_NEW_GAMEMODE},
    selection_tree_row{T_CLONE_GAMEMODE},
    selection_tree_row{T_EDIT_GAMEMODE},
    selection_tree_row{T_DELETE_GAMEMODES},
	selection_tree_row{T_EXIT},
};

// Shortcut table for the gamemode manager menu.
constexpr shortcut_table SHORTCUTS{
    {"N"_kc, T_NEW_GAMEMODE}, {"1"_kc, T_NEW_GAMEMODE},
    {"C"_kc, T_CLONE_GAMEMODE}, {"2"_kc, T_CLONE_GAMEMODE},
    {"E"_kc, T_EDIT_GAMEMODE}, {"3"_kc, T_EDIT_GAMEMODE},
    {"D"_kc, T_DELETE_GAMEMODES}, {"4"_kc, T_DELETE_GAMEMODES},
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT}, {"5"_kc, T_EXIT},
};

// clang-format on
////////////////////////////////////////////////////////// GAMEMODE MANAGER STATE /////////////////////////////////////////////////////////

gamemode_manager_state::gamemode_manager_state(std::shared_ptr<playerless_game> game, animate_title animate_title)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_GAMEMODE_MANAGER}
{
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = bool(animate_title) ? tweened_position{TOP_START_POS, TITLE_POS, 0.5_s} : tweened_position{TITLE_POS},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = bool(animate_title) ? 0.5_s : 0,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});

	const std::array<action_command, CENTER_BUTTONS.size()> center_actions{
		[this] { on_enter_new_gamemode(); },
		[this] { on_enter_clone_gamemode(); },
		[this] { on_enter_edit_gamemode(); },
		[this] { on_enter_delete_gamemode(); },
	};
	for (usize i = 0; i < CENTER_BUTTONS.size(); ++i) {
		const float y{500.0f - ((CENTER_BUTTONS.size() - 1) * 50.0f) + i * 100};
		m_ui.emplace<text_button_widget>(CENTER_BUTTONS[i].tag, {
			.animation = {glm::vec2{i % 2 == 0 ? 600 : 400, y}, glm::vec2{500, y}, 0.5_s},
			.tooltip_text = localized_text{CENTER_BUTTONS[i].tooltip},
			.text = localized_text{CENTER_BUTTONS[i].tag},
			.font_size = 64,
			.status = [this] { return m_substate == substate::IN_GAMEMODE_MANAGER; },
			.action = center_actions[i],
		});
	}

	m_ui.emplace<text_button_widget>(T_EXIT, {
		.animation = {BOTTOM_START_POS, {500, 1000}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.text = localized_text{T_EXIT},
		.status = [this] { return m_substate == substate::IN_GAMEMODE_MANAGER; },
		.action = [this] { on_exit(); },
		.action_sound = sound::CANCEL
	});
	// clang-format on
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

//

void gamemode_manager_state::on_enter_new_gamemode()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_title::NO);
	m_next_state =
		make_async<gamemode_editor_state>(m_game, new_gamemode_editor{}, savefile::instance().gamemode_draft, animate_subtitle::YES);
}

void gamemode_manager_state::on_enter_edit_gamemode()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_title::NO);
	m_next_state = make_async<gamemode_selector_state>(m_game, edit_gamemode_selector{}, animate_subtitle::YES);
}

void gamemode_manager_state::on_enter_clone_gamemode()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_title::NO);
	m_next_state = make_async<gamemode_selector_state>(m_game, clone_gamemode_selector{}, animate_subtitle::YES);
}

void gamemode_manager_state::on_enter_delete_gamemode()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_title::NO);
	m_next_state = make_async<gamemode_selector_state>(m_game, delete_gamemodes_selector{}, animate_subtitle::YES);
}

void gamemode_manager_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_title::YES);
	m_next_state = make_async<title_state>(m_game);
}