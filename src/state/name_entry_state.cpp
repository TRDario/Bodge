#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

//

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

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 1.0_s};
constexpr tweener<glm::vec2> CONFIRM_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 1.0_s};

//

name_entry_state::name_entry_state()
	: main_menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::FADING_IN}
{
}

//

void name_entry_state::set_up_ui()
{
	using enum tr::align;

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 1.0_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	m_ui.emplace<line_input_widget<20>>(T_INPUT, glm::vec2{500, 500}, CENTER, 1.0_s, text_style::NORMAL, 64, input_interactible, on_confirm,
										std::string_view{});
	m_ui.emplace<text_button_widget>(T_CONFIRM, CONFIRM_MOVE_IN, BOTTOM_CENTER, 1.0_s, NO_TOOLTIP, tag_loc{T_CONFIRM}, font::LANGUAGE, 48,
									 confirm_interactible, on_confirm, sound::CONFIRM);

	m_ui.select_widget(T_INPUT);
}

next_state name_entry_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::FADING_IN:
		if (m_timer >= 1.0_s) {
			m_timer = 0;
			m_substate = substate::IN_NAME_ENTRY;
		}
		return tr::KEEP_STATE;
	case substate::IN_NAME_ENTRY:
		return tr::KEEP_STATE;
	case substate::ENTERING_TITLE:
		return m_timer >= 1.0_s ? g_next_state.get() : tr::KEEP_STATE;
	}
}

//

float name_entry_state::fade_overlay_opacity()
{
	return m_substate == substate::FADING_IN ? 1 - m_timer / 1.0_sf : 0;
}

//

bool name_entry_state::input_interactible()
{
	const name_entry_state& self{g_state_machine.get<name_entry_state>()};

	return self.m_substate != substate::ENTERING_TITLE;
}

bool name_entry_state::confirm_interactible()
{
	const name_entry_state& self{g_state_machine.get<name_entry_state>()};

	return self.m_substate != substate::ENTERING_TITLE && !self.m_ui.as<line_input_widget<20>>(T_INPUT).buffer.empty();
}

void name_entry_state::on_confirm()
{
	name_entry_state& self{g_state_machine.get<name_entry_state>()};

	line_input_widget<20>& input{self.m_ui.as<line_input_widget<20>>(T_INPUT)};
	if (!input.buffer.empty()) {
		self.m_timer = 0;
		self.m_substate = substate::ENTERING_TITLE;
		self.m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 1.0_s);
		self.m_ui[T_CONFIRM].pos.change(tween::CUBIC, BOTTOM_START_POS, 1.0_s);
		self.m_ui.hide_all_widgets(1.0_s);
		g_scorefile.name = input.buffer;
		prepare_next_state<title_state>(self.m_game);
	}
}