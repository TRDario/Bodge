#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

//

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
	{"Enter"_kc, T_EXIT},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

//

credits_state::credits_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_CREDITS}
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_CREDITS; },
	};

	// ACTION CALLBACKS

	const action_callback exit_acb{
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
			m_ui[T_EXIT].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
			m_ui.hide_all_widgets(0.5_s);
			m_next_state = make_async<title_state>(m_game);
		},
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   text_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_BODGE, glm::vec2{400, 210}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP, string_text_callback{T_BODGE},
							   text_style::NORMAL, 128);
	m_ui.emplace<label_widget>(T_VERSION, glm::vec2{600, 290}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP, string_text_callback{T_VERSION},
							   text_style::NORMAL, 32);
	m_ui.emplace<label_widget>(T_DEVELOPED_BY, glm::vec2{400, 410}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP,
							   loc_text_callback{T_DEVELOPED_BY}, text_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_TRDARIO, glm::vec2{600, 470}, tr::align::CENTER, DONT_UNHIDE, loc_text_callback{"trdario_tt"},
							   string_text_callback{T_TRDARIO}, text_style::NORMAL, 48, "FF8080A0"_rgba8);
	m_ui.emplace<image_widget>(T_ART, glm::vec2{500, 550}, tr::align::TOP_CENTER, DONT_UNHIDE, 0, "credits_art");
	m_ui.emplace<label_widget>(T_PLAYTESTERS, glm::vec2{400, 710}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP,
							   loc_text_callback{T_PLAYTESTERS}, text_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_STARSURGE, glm::vec2{600, 770}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP,
							   string_text_callback{T_STARSURGE}, text_style::NORMAL, 48);
	m_ui.emplace<label_widget>(T_TOWELI, glm::vec2{400, 820}, tr::align::CENTER, DONT_UNHIDE, NO_TOOLTIP, string_text_callback{T_TOWELI},
							   text_style::NORMAL, 48);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

tr::next_state credits_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_CREDITS:
		switch (m_timer) {
		case 1: {
			widget& bodge{m_ui[T_BODGE]};
			widget& version{m_ui[T_VERSION]};
			bodge.unhide(3_s);
			bodge.pos.change(tween::CUBIC, {500, glm::vec2{bodge.pos}.y}, 3_s);
			version.unhide(3_s);
			version.pos.change(tween::CUBIC, {500, glm::vec2{version.pos}.y}, 3_s);
		} break;
		case 1_s: {
			widget& developed_by{m_ui[T_DEVELOPED_BY]};
			developed_by.unhide(4_s);
			developed_by.pos.change(tween::CUBIC, {500, glm::vec2{developed_by.pos}.y}, 4_s);
		} break;
		case 2_s: {
			widget& trdario{m_ui[T_TRDARIO]};
			trdario.unhide(4_s);
			trdario.pos.change(tween::CUBIC, {500, glm::vec2{trdario.pos}.y}, 4_s);
		} break;
		case 3_s: {
			widget& art{m_ui[T_ART]};
			art.unhide(4_s);
			art.pos.change(tween::CUBIC, {500, glm::vec2{art.pos}.y - 50}, 4_s);
		} break;
		case 4_s: {
			widget& playtesters{m_ui[T_PLAYTESTERS]};
			playtesters.unhide(4_s);
			playtesters.pos.change(tween::CUBIC, {500, glm::vec2{playtesters.pos}.y}, 4_s);
		} break;
		case 5_s: {
			widget& starsurge{m_ui[T_STARSURGE]};
			starsurge.unhide(4_s);
			starsurge.pos.change(tween::CUBIC, {500, glm::vec2{starsurge.pos}.y}, 4_s);
		} break;
		case 6_s: {
			widget& toweli{m_ui[T_TOWELI]};
			toweli.unhide(4_s);
			toweli.pos.change(tween::CUBIC, {500, glm::vec2{toweli.pos}.y}, 4_s);
		} break;
		default:
			break;
		}
		return tr::KEEP_STATE;
	case substate::ENTERING_TITLE:
		return next_state_if_after(0.5_s);
	};
}