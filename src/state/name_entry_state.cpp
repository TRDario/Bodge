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
	{{tr::system::keycode::ENTER}, T_CONFIRM},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 1.0_s};
constexpr tweener<glm::vec2> CONFIRM_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 1.0_s};

//

name_entry_state::name_entry_state()
	: main_menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::FADING_IN}
{
	// STATUS CALLBACKS

	const status_callback input_scb{
		[this] { return m_substate != substate::ENTERING_TITLE; },
	};
	const status_callback confirm_scb{
		[this] { return m_substate != substate::ENTERING_TITLE && !m_ui.as<line_input_widget<20>>(T_INPUT).buffer.empty(); },
	};

	// ACTION CALLBACKS

	const action_callback action_cb{
		[this] {
			line_input_widget<20>& input{m_ui.as<line_input_widget<20>>(T_INPUT)};
			if (!input.buffer.empty()) {
				m_timer = 0;
				m_substate = substate::ENTERING_TITLE;
				m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 1.0_s);
				m_ui[T_CONFIRM].pos.change(tween::CUBIC, BOTTOM_START_POS, 1.0_s);
				m_ui.hide_all_widgets(1.0_s);
				engine::play_sound(sound::CONFIRM, 0.5f, 0.0f);
				engine::scorefile.name = input.buffer;
			}
		},
	};

	//

	engine::play_song("menu", 1.0s);

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 1.0_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<line_input_widget<20>>(T_INPUT, glm::vec2{500, 500}, tr::align::CENTER, 1.0_s, tr::system::ttf_style::NORMAL, 64,
										input_scb, action_cb, std::string_view{});
	m_ui.emplace<text_button_widget>(T_CONFIRM, CONFIRM_MOVE_IN, tr::align::BOTTOM_CENTER, 1.0_s, NO_TOOLTIP, loc_text_callback{T_CONFIRM},
									 font::LANGUAGE, 48, confirm_scb, action_cb, sound::CONFIRM);

	m_ui.select_widget(T_INPUT);
}

//

std::unique_ptr<tr::state> name_entry_state::update(tr::duration)
{
	main_menu_state::update({});
	switch (m_substate) {
	case substate::FADING_IN:
		if (m_timer >= 1.0_s) {
			m_timer = 0;
			m_substate = substate::IN_NAME_ENTRY;
		}
		return nullptr;
	case substate::IN_NAME_ENTRY:
		return nullptr;
	case substate::ENTERING_TITLE:
		return m_timer >= 1.0_s ? std::make_unique<title_state>(release_game()) : nullptr;
	}
}

//

float name_entry_state::fade_overlay_opacity()
{
	return m_substate == substate::FADING_IN ? 1 - m_timer / 1.0_sf : 0;
}