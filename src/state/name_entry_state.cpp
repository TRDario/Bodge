///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements name_entry_state from state.hpp.                                                                                           //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"enter_your_name"};
constexpr tag T_INPUT{"input"};
constexpr tag T_CONFIRM{"confirm"};

// Selection tree for the name entry screen.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_INPUT},
	selection_tree_row{T_CONFIRM},
};

// Shortcut table for the name entry screen.
constexpr shortcut_table SHORTCUTS{
	{"Enter"_kc, T_CONFIRM},
};

//////////////////////////////////////////////////////////// NAME ENTRY STATE /////////////////////////////////////////////////////////////

name_entry_state::name_entry_state()
	: main_menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::FADING_IN}
{
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = {TOP_START_POS, TITLE_POS, 1.0_s},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = 1.0_s,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<line_input_widget<20>>(T_INPUT, {
		.animation = {{500, 500}},
		.unhide_time = 1.0_s,
		.font_size = 64,
		.status = [this] { return m_substate != substate::EXITING; },
		.enter_action = [this] { on_exit(); }
	});
	m_ui.emplace<text_button_widget>(T_CONFIRM, {
		.animation = {BOTTOM_START_POS, {500, 1000}, 1.0_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.unhide_time = 1.0_s,
		.text = localized_text{T_CONFIRM},
		.status = [this] { return m_substate != substate::EXITING && !m_ui.as<line_input_widget<20>>(T_INPUT).contents().empty(); },
		.action = [this] { on_exit(); }
	});
	// clang-format on

	m_ui.select_widget(T_INPUT);
}

//

tr::next_state name_entry_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::FADING_IN:
		if (m_elapsed == 1) {
			g_audio.play_song("menu", 1.0s);
		}
		else if (m_elapsed >= 1.0_s) {
			m_elapsed = 0;
			m_substate = substate::IN_NAME_ENTRY;
		}
		return tr::KEEP_STATE;
	case substate::IN_NAME_ENTRY:
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(1.0_s);
	}
}

//

float name_entry_state::fade_overlay_opacity()
{
	return m_substate == substate::FADING_IN ? 1 - m_elapsed / 1.0_sf : 0;
}

//

void name_entry_state::on_exit()
{
	const std::string_view name{m_ui.as<line_input_widget<20>>(T_INPUT).contents()};
	if (!name.empty()) {
		m_elapsed = 0;
		m_substate = substate::EXITING;
		m_ui[T_TITLE].move_and_hide(TOP_START_POS, 1.0_s);
		m_ui[T_INPUT].hide(1.0_s);
		m_ui[T_CONFIRM].move_and_hide(BOTTOM_START_POS, 1.0_s);
		g_scorefile.name = name;
		m_next_state = make_async<title_state>(m_game);
	}
}