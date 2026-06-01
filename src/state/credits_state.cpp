///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements credits_state from state.hpp.                                                                                              //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"credits"};
constexpr tag T_BODGE{"Bodge"};
constexpr tag T_VERSION{VERSION_STRING};
constexpr tag T_DEVELOPED_BY{"developed_by"};
constexpr tag T_TRDARIO{"TRDario"};
constexpr tag T_ART{"art"};
constexpr tag T_PLAYTESTERS{"playtesters"};
constexpr tag T_STARSURGE{"starsurge"};
constexpr tag T_TOWELI{"toweli"};
constexpr tag T_ZER0{"Zer0"};
constexpr tag T_EXIT{"exit"};

// Selection tree for the credits screen.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_EXIT},
};

// Shortcut table for the credits screen.
constexpr shortcut_table SHORTCUTS{
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT}, {"1"_kc, T_EXIT}
};

// clang-format on
////////////////////////////////////////////////////////////// CREDITS STATE //////////////////////////////////////////////////////////////

credits_state::credits_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_CREDITS}
{
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = {TOP_START_POS, TITLE_POS, 0.5_s},
		.alignment = tr::align::TOP_CENTER,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_BODGE, {
		.animation = {{400, 185}},
		.unhide_time = DONT_UNHIDE,
		.text = constant_text{T_BODGE},
		.font_size = 128
	});
	m_ui.emplace<label_widget>(T_VERSION, {
		.animation = {{600, 265}},
		.unhide_time = DONT_UNHIDE,
		.text = constant_text{T_VERSION},
	});
	m_ui.emplace<label_widget>(T_DEVELOPED_BY, {
		.animation = {{400, 385}},
		.unhide_time = DONT_UNHIDE,
		.text = localized_text{T_DEVELOPED_BY},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_TRDARIO, {
		.animation = {{600, 445}},
		.unhide_time = DONT_UNHIDE,
		.tooltip_text = localized_text{"trdario_tt"},
		.text = constant_text{T_TRDARIO},
		.color = "FF8080A0"_rgba8
	});
	m_ui.emplace<image_widget>(T_ART, {
		.animation = {{500, 525}},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = DONT_UNHIDE,
		.file = "credits_art"
	});
	m_ui.emplace<label_widget>(T_PLAYTESTERS, {
		.animation = {{400, 685}},
		.unhide_time = DONT_UNHIDE,
		.text = localized_text{T_PLAYTESTERS},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_STARSURGE, {
		.animation = {{600, 745}},
		.unhide_time = DONT_UNHIDE,
		.text = constant_text{T_STARSURGE}
	});
	m_ui.emplace<label_widget>(T_TOWELI, {
		.animation = {{400, 795}},
		.unhide_time = DONT_UNHIDE,
		.text = constant_text{T_TOWELI},
	});
	m_ui.emplace<label_widget>(T_ZER0, {
		.animation = {{600, 845}},
		.unhide_time = DONT_UNHIDE,
		.tooltip_text = localized_text{"zer0_tt"},
		.text = constant_text{T_ZER0}
	});
	m_ui.emplace<text_button_widget>(T_EXIT, {
		.animation = {BOTTOM_START_POS, {500, 1000}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.text = localized_text{T_EXIT},
		.status = [this] { return m_substate == substate::IN_CREDITS; },
		.action = [this] { on_exit(); },
		.action_sound = sound::CANCEL
	});
	// clang-format on
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
			m_ui[T_ART].move_y_and_unhide(475, 4_s);
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
		case 7_s:
			m_ui[T_ZER0].move_x_and_unhide(500, 4_s);
			break;
		}
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	};
}

//

void credits_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	m_ui[T_TITLE].move_and_hide(TOP_START_POS, 0.5_s);
	m_ui[T_BODGE].hide(0.5_s);
	m_ui[T_VERSION].hide(0.5_s);
	m_ui[T_DEVELOPED_BY].hide(0.5_s);
	m_ui[T_TRDARIO].hide(0.5_s);
	m_ui[T_ART].hide(0.5_s);
	m_ui[T_PLAYTESTERS].hide(0.5_s);
	m_ui[T_STARSURGE].hide(0.5_s);
	m_ui[T_TOWELI].hide(0.5_s);
	m_ui[T_ZER0].hide(0.5_s);
	m_ui[T_EXIT].move_and_hide(BOTTOM_START_POS, 0.5_s);
	m_next_state = make_async<title_state>(m_game);
}