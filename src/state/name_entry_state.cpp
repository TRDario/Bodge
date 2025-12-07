#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"enter_your_name"};
constexpr tag T_INPUT{"input"};
constexpr tag T_CONFIRM{"confirm"};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_INPUT},
	selection_tree_row{T_CONFIRM},
};

constexpr shortcut_table SHORTCUTS{
	{"Enter"_kc, T_CONFIRM},
};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 1.0_s};
constexpr tweened_position CONFIRM_MOVE_IN{BOTTOM_START_POS, {500, 1000}, 1.0_s};

//////////////////////////////////////////////////////////// NAME ENTRY STATE /////////////////////////////////////////////////////////////

name_entry_state::name_entry_state()
	: main_menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::FADING_IN}
{
	// STATUS CALLBACKS

	const status_callback input_scb{[this] { return m_substate != substate::EXITING; }};
	const status_callback confirm_scb{
		[this] { return m_substate != substate::EXITING && !m_ui.as<line_input_widget<20>>(T_INPUT).buffer.empty(); },
	};

	// ACTION CALLBACKS

	const action_callback action_cb{[this] {
		const tr::static_string<80>& name{m_ui.as<line_input_widget<20>>(T_INPUT).buffer};
		if (!name.empty()) {
			m_elapsed = 0;
			m_substate = substate::EXITING;
			m_ui[T_TITLE].pos.move(TOP_START_POS, 1.0_s);
			m_ui[T_CONFIRM].pos.move(BOTTOM_START_POS, 1.0_s);
			m_ui.hide_all_widgets(1.0_s);
			g_scorefile.name = name;
			m_next_state = make_async<title_state>(m_game);
		}
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 1.0_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<line_input_widget<20>>(T_INPUT, glm::vec2{500, 500}, tr::align::CENTER, 1.0_s, tr::sys::ttf_style::NORMAL, 64, input_scb,
										action_cb);
	m_ui.emplace<text_button_widget>(T_CONFIRM, CONFIRM_MOVE_IN, tr::align::BOTTOM_CENTER, 1.0_s, NO_TOOLTIP, loc_text_callback{T_CONFIRM},
									 font::LANGUAGE, 48, confirm_scb, action_cb, sound::CONFIRM);

	m_ui.select_widget(T_INPUT);
}

//

tr::next_state name_entry_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::FADING_IN:
		if (m_elapsed >= 1.0_s) {
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