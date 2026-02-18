#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"credits"};
constexpr tag T_BODGE{"Bodge"};
constexpr tag T_VERSION{VERSION_STRING};
constexpr tag T_DEVELOPED_BY{"developed_by"};
constexpr tag T_TRDARIO{"TRDario"};
constexpr tag T_ART{"art"};
constexpr tag T_PLAYTESTERS{"playtesters"};
constexpr tag T_STARSURGE{"starsurge"};
constexpr tag T_TOWELI{"toweli"};
constexpr tag T_EXIT{"exit"};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{"Escape"_kc, T_EXIT},
};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweened_position EXIT_ANIMATION{BOTTOM_START_POS, {500, 1000}, 0.5_s};

////////////////////////////////////////////////////////////// CREDITS STATE //////////////////////////////////////////////////////////////

credits_state::credits_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_CREDITS}
{
	// STATUS CALLBACKS

	const status_callback scb{[this] { return m_substate == substate::IN_CREDITS; }};

	// ACTION CALLBACKS

	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING;
		m_elapsed = 0;
		m_ui[T_TITLE].pos.move(TOP_START_POS, 0.5_s);
		m_ui[T_EXIT].pos.move(BOTTOM_START_POS, 0.5_s);
		m_ui.hide_all_widgets(0.5_s);
		m_next_state = make_async<title_state>(m_game);
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_BODGE, glm::vec2{400, 210}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP, const_text_callback{T_BODGE},
							   tr::sys::ttf_style::NORMAL, 128);
	m_ui.emplace<label_widget>(T_VERSION, glm::vec2{600, 290}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP, const_text_callback{T_VERSION},
							   tr::sys::ttf_style::NORMAL, 32);
	m_ui.emplace<label_widget>(T_DEVELOPED_BY, glm::vec2{400, 410}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP,
							   loc_text_callback{T_DEVELOPED_BY}, tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_TRDARIO, glm::vec2{600, 470}, tr::align::CENTER, DONT_UNHIDE, loc_text_callback{"trdario_tt"},
							   const_text_callback{T_TRDARIO}, tr::sys::ttf_style::NORMAL, 48, "FF8080A0"_rgba8);
	m_ui.emplace<image_widget>(T_ART, glm::vec2{500, 550}, tr::align::TOP_CENTER, DONT_UNHIDE, 0, "credits_art");
	m_ui.emplace<label_widget>(T_PLAYTESTERS, glm::vec2{400, 710}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP,
							   loc_text_callback{T_PLAYTESTERS}, tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_STARSURGE, glm::vec2{600, 770}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP,
							   const_text_callback{T_STARSURGE}, tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<label_widget>(T_TOWELI, glm::vec2{400, 820}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP, const_text_callback{T_TOWELI},
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_ANIMATION, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

tr::next_state credits_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_CREDITS:
		switch (m_elapsed) {
		case 1:
			m_ui[T_BODGE].move_x_and_unhide(500, 3_s - 1);
			m_ui[T_VERSION].move_x_and_unhide(500, 3_s - 1);
			break;
		case 1_s:
			m_ui[T_DEVELOPED_BY].move_x_and_unhide(500, 4_s);
			break;
		case 2_s:
			m_ui[T_TRDARIO].move_x_and_unhide(500, 4_s);
			break;
		case 3_s:
			m_ui[T_ART].move_y_and_unhide(500, 4_s);
			break;
		case 4_s:
			m_ui[T_PLAYTESTERS].move_x_and_unhide(500, 4_s);
			break;
		case 5_s:
			m_ui[T_STARSURGE].move_x_and_unhide(500, 4_s);
			break;
		case 6_s:
			m_ui[T_TOWELI].move_x_and_unhide(500, 4_s);
			break;
		}
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	};
}