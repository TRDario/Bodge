///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements scoreboard_state from state.hpp.                                                                                           //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

// Number of scores displayed per page.
constexpr int SCORES_PER_PAGE{8};

constexpr tag T_TITLE{"scoreboards"};
constexpr tag T_PLAYER_INFO{"player_info"};
constexpr tag T_NO_SCORES_FOUND{"no_scores_found"};
constexpr tag T_SCORE_0{"score0"};
constexpr tag T_SCORE_1{"score1"};
constexpr tag T_SCORE_2{"score2"};
constexpr tag T_SCORE_3{"score3"};
constexpr tag T_SCORE_4{"score4"};
constexpr tag T_SCORE_5{"score5"};
constexpr tag T_SCORE_6{"score6"};
constexpr tag T_SCORE_7{"score7"};
constexpr tag T_GAMEMODE_D{"gamemode_d"};
constexpr tag T_GAMEMODE_C{"gamemode_c"};
constexpr tag T_GAMEMODE_I{"gamemode_i"};
constexpr tag T_PAGE_D{"page_d"};
constexpr tag T_PAGE_C{"page_c"};
constexpr tag T_PAGE_I{"page_i"};
constexpr tag T_EXIT{"exit"};

// Tags of the score widgets.
constexpr std::array SCORE_TAGS{T_SCORE_0, T_SCORE_1, T_SCORE_2, T_SCORE_3, T_SCORE_4, T_SCORE_5, T_SCORE_6, T_SCORE_7};

// Selection tree used for the scoreboard screen.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_EXIT},
};

// Shortcut table used for the scoreboard screen.
constexpr shortcut_table SHORTCUTS{
	{"Left"_kc, T_PAGE_D},
	{"Right"_kc, T_PAGE_I},
	{"Shift+Left"_kc, T_GAMEMODE_D},
	{"Shift+Right"_kc, T_GAMEMODE_I},
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT},
};

// Entry animation used for the "no scores found" widget.
constexpr tweened_position NO_SCORES_FOUND_ANIMATION{{600, 500}, {500, 500}, 0.5_s};
// Entry animation used for the previous gamemode button widget.
constexpr tweened_position GAMEMODE_D_ANIMATION{{-50, 892.5}, {10, 892.5}, 0.5_s};
// Entry animation used for the current gamemode widget.
constexpr tweened_position GAMEMODE_C_ANIMATION{BOTTOM_START_POS, {500, 900}, 0.5_s};
// Entry animation used for the next gamemode button widget.
constexpr tweened_position GAMEMODE_I_ANIMATION{{1050, 892.5}, {990, 892.5}, 0.5_s};
// Entry animation used for the previous page button widget.
constexpr tweened_position PAGE_D_ANIMATION{{-50, 942.5}, {10, 942.5}, 0.5_s};
// Entry animation used for the current page widget.
constexpr tweened_position PAGE_C_ANIMATION{BOTTOM_START_POS, {500, 950}, 0.5_s};
// Entry animation used for the next page button widget.
constexpr tweened_position PAGE_I_ANIMATION{{1050, 942.5}, {990, 942.5}, 0.5_s};

// clang-format on
///////////////////////////////////////////////////////////// INTERNAL HELPERS ////////////////////////////////////////////////////////////

// Creates a set of widgets for a new page of scores.
static std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets(enum score_widget::type type,
																			 const std::vector<score_entry>& scores, int page)
{
	std::unordered_map<tag, std::unique_ptr<widget>> map;
	for (usize i = 0; i < SCORES_PER_PAGE; ++i) {
		const tweened_position animation{{i % 2 == 0 ? 600 : 400, 173 + 86 * i}, {500, 173 + 86 * i}, 0.25_s};
		const usize rank{page * SCORES_PER_PAGE + i + 1};
		const tr::opt_ref<const score_entry> score{rank <= scores.size() ? tr::opt_ref{scores[rank - 1]} : std::nullopt};
		map.emplace(SCORE_TAGS[i], std::make_unique<score_widget>(animation, tr::align::CENTER, 0.25_s, type, rank, score));
	}
	return map;
}

///////////////////////////////////////////////////////////// SCOREBOARD STATE ////////////////////////////////////////////////////////////

scoreboard_state::scoreboard_state(std::shared_ptr<playerless_game> game, scoreboard scoreboard)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_SCOREBOARD}
	, m_scoreboard{scoreboard}
	, m_page{0}
	, m_selected{g_scorefile.categories.begin()}
{
	if (!g_scorefile.categories.empty()) {
		m_sorted_scores = m_selected->entries;
		std::ranges::sort(m_sorted_scores, scoreboard == scoreboard::SCORE ? compare_scores : compare_times);
	}

	// TOOLTIP CALLBACKS

	const text_callback cur_gamemode_ttcb{[this] { return std::string{m_selected->gamemode.description_loc()}; }};

	// STATUS CALLBACKS

	const status_callback scb{[this] { return m_substate == substate::IN_SCOREBOARD; }};
	const status_callback gamemode_change_scb{
		[this] { return m_substate == substate::IN_SCOREBOARD && g_scorefile.categories.size() > 1; },
	};
	const status_callback page_d_scb{[this] { return m_substate == substate::IN_SCOREBOARD && m_page > 0; }};
	const status_callback page_i_scb{[this] {
		const int last_page{std::max(int(m_selected->entries.size()) - 1, 0) / SCORES_PER_PAGE};
		return m_substate == substate::IN_SCOREBOARD && m_page < last_page;
	}};

	// ACTION CALLBACKS

	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING;
		m_elapsed = 0;
		set_up_exit_animation();
		m_next_state = make_async<scoreboard_selection_state>(m_game, animate_title::NO);
	}};
	const action_callback gamemode_d_acb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_elapsed = 0;
		m_page = 0;
		if (m_selected == g_scorefile.categories.begin()) {
			m_selected = g_scorefile.categories.end();
		}
		--m_selected;
		set_up_page_switch_animation();
	}};
	const action_callback gamemode_i_acb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_elapsed = 0;
		m_page = 0;
		if (++m_selected == g_scorefile.categories.end()) {
			m_selected = g_scorefile.categories.begin();
		}
		set_up_page_switch_animation();
	}};
	const action_callback page_d_acb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_elapsed = 0;
		--m_page;
		set_up_page_switch_animation();
	}};
	const action_callback page_i_acb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_elapsed = 0;
		++m_page;
		set_up_page_switch_animation();
	}};

	// TEXT CALLBACKS

	const text_callback cur_gamemode_tcb{[this] { return std::string{m_selected->gamemode.name_loc()}; }};
	const text_callback cur_page_tcb{[this] {
		const int total{std::max(int(m_selected->entries.size()) - 1, 0) / SCORES_PER_PAGE + 1};
		return TR_FMT::format("{}/{}", m_page + 1, total);
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_PLAYER_INFO, glm::vec2{500, 64}, tr::align::TOP_CENTER, 0, NO_TOOLTIP,
							   const_text_callback{g_scorefile.format_player_info()}, tr::sys::ttf_style::NORMAL, 32);
	m_ui.emplace<text_button_widget>(T_EXIT, glm::vec2{500, 1000}, tr::align::BOTTOM_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	if (g_scorefile.categories.empty()) {
		m_ui.emplace<label_widget>(T_NO_SCORES_FOUND, NO_SCORES_FOUND_ANIMATION, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
								   loc_text_callback{T_NO_SCORES_FOUND}, tr::sys::ttf_style::NORMAL, 64, DARK_GRAY);
		return;
	}
	for (usize i = 0; i < SCORES_PER_PAGE; ++i) {
		const tweened_position animation{{i % 2 == 0 ? 400 : 600, 173 + 86 * i}, {500, 173 + 86 * i}, 0.5_s};
		const usize rank{m_page * SCORES_PER_PAGE + i + 1};
		const tr::opt_ref<score_entry> score{m_sorted_scores.size() > i ? tr::opt_ref{m_sorted_scores[i]} : std::nullopt};
		m_ui.emplace<score_widget>(SCORE_TAGS[i], animation, tr::align::CENTER, 0.5_s, (enum score_widget::type)(m_scoreboard), rank,
								   score);
	}
	m_ui.emplace<arrow_widget>(T_GAMEMODE_D, GAMEMODE_D_ANIMATION, tr::valign::BOTTOM, 0.5_s, arrow_type::LEFT, gamemode_change_scb,
							   gamemode_d_acb);
	m_ui.emplace<label_widget>(T_GAMEMODE_C, GAMEMODE_C_ANIMATION, tr::align::BOTTOM_CENTER, 0.5_s, cur_gamemode_ttcb, cur_gamemode_tcb,
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_GAMEMODE_I, GAMEMODE_I_ANIMATION, tr::valign::BOTTOM, 0.5_s, arrow_type::RIGHT, gamemode_change_scb,
							   gamemode_i_acb);
	m_ui.emplace<arrow_widget>(T_PAGE_D, PAGE_D_ANIMATION, tr::valign::BOTTOM, 0.5_s, arrow_type::LEFT, page_d_scb, page_d_acb);
	m_ui.emplace<label_widget>(T_PAGE_C, PAGE_C_ANIMATION, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, cur_page_tcb,
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_PAGE_I, PAGE_I_ANIMATION, tr::valign::BOTTOM, 0.5_s, arrow_type::RIGHT, page_i_scb, page_i_acb);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

tr::next_state scoreboard_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_SCOREBOARD:
		return tr::KEEP_STATE;
	case substate::SWITCHING_PAGE:
		if (m_elapsed >= 0.5_s) {
			m_elapsed = 0;
			m_substate = substate::IN_SCOREBOARD;
		}
		else if (m_elapsed == 0.25_s) {
			m_ui.replace(m_next_widgets.get());
		}
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void scoreboard_state::set_up_page_switch_animation()
{
	for (usize i = 0; i < SCORES_PER_PAGE; i++) {
		m_ui[SCORE_TAGS[i]].move_x_and_hide(i % 2 == 0 ? 600 : 400, 0.25_s);
	}
	m_sorted_scores = m_selected->entries;
	std::ranges::sort(m_sorted_scores, m_scoreboard == scoreboard::SCORE ? compare_scores : compare_times);
	m_next_widgets = std::async(std::launch::async, prepare_next_widgets, (enum score_widget::type)(m_scoreboard), m_sorted_scores, m_page);
}

void scoreboard_state::set_up_exit_animation()
{
	if (g_scorefile.categories.empty()) {
		m_ui[T_NO_SCORES_FOUND].move_x_and_hide(400, 0.5_s);
	}
	else {
		for (usize i = 0; i < SCORES_PER_PAGE; i++) {
			m_ui[SCORE_TAGS[i]].move_x_and_hide(i % 2 == 0 ? 600 : 400, 0.5_s);
		}
		m_ui[T_GAMEMODE_D].move_x_and_hide(-50, 0.5_s);
		m_ui[T_GAMEMODE_C].move_and_hide(BOTTOM_START_POS, 0.5_s);
		m_ui[T_GAMEMODE_I].move_x_and_hide(1050, 0.5_s);
		m_ui[T_PAGE_D].move_x_and_hide(-50, 0.5_s);
		m_ui[T_PAGE_C].move_and_hide(BOTTOM_START_POS, 0.5_s);
		m_ui[T_PAGE_I].move_x_and_hide(1050, 0.5_s);
	}
}