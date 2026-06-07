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

// clang-format on
///////////////////////////////////////////////////////////// INTERNAL HELPERS ////////////////////////////////////////////////////////////

// Creates a set of widgets for a new page of scores.
static std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets(const localization& localization, enum score_widget::type type,
																			 const std::vector<score_entry>& scores, int page)
{
	std::unordered_map<tag, std::unique_ptr<widget>> map;
	for (usize i = 0; i < SCORES_PER_PAGE; ++i) {
		const usize rank{page * SCORES_PER_PAGE + i + 1};
		// clang-format off
		map.emplace(SCORE_TAGS[i], std::make_unique<score_widget>(score_widget::properties{
			.localization = localization,
			.animation = {{i % 2 == 0 ? 600 : 400, 173 + 86 * i}, {500, 173 + 86 * i}, 0.25_s},
			.unhide_time = 0.25_s,
			.type = type,
			.rank = rank,
			.score = rank <= scores.size() ? tr::opt_ref{scores[rank - 1]} : std::nullopt
		}));
		// clang-format on
	}
	return map;
}

///////////////////////////////////////////////////////////// SCOREBOARD STATE ////////////////////////////////////////////////////////////

scoreboard_state::scoreboard_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, savefile savefile,
								   scoreboard scoreboard)
	: main_menu_state{std::move(subsystems), SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_SCOREBOARD}
	, m_scoreboard{scoreboard}
	, m_page{0}
	, m_savefile{std::move(savefile)}
	, m_selected{m_savefile.score_categories().begin()}
{
	if (!m_savefile.score_categories().empty()) {
		m_sorted_scores = m_selected->entries;
		std::ranges::sort(m_sorted_scores, scoreboard == scoreboard::SCORE ? compare_scores : compare_times);
	}

	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = TITLE_POS,
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = 0_s,
		.text = localized_text{m_subsystems->localization, T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_PLAYER_INFO, {
		.animation = {{500, 64}},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = 0_s,
		.text = constant_text{m_savefile.format_info(m_subsystems->localization)},
		.font_size = 32
	});
	m_ui.emplace<text_button_widget>(T_EXIT, {
		.animation = {{500, 1000}},
		.alignment = tr::align::BOTTOM_CENTER,
		.unhide_time = 0_s,
		.text = localized_text{m_subsystems->localization, T_EXIT},
		.status = [this] { return m_substate == substate::IN_SCOREBOARD; },
		.action = [this] { on_exit(); },
		.action_sound = sound::CANCEL
	});

	if (m_savefile.score_categories().empty()) {
		m_ui.emplace<label_widget>(T_NO_SCORES_FOUND, {
			.animation = {{600, 500}, {500, 500}, 0.5_s},
			.text = localized_text{m_subsystems->localization, T_NO_SCORES_FOUND},
			.font_size = 64,
			.color = DARK_GRAY
		});
		return;
	}

	for (usize i = 0; i < SCORES_PER_PAGE; ++i) {
		m_ui.emplace<score_widget>(SCORE_TAGS[i], {
			.localization = m_subsystems->localization,
			.animation = {{i % 2 == 0 ? 400 : 600, 173 + 86 * i}, {500, 173 + 86 * i}, 0.5_s},
			.type = (enum score_widget::type)(m_scoreboard),
			.rank = m_page * SCORES_PER_PAGE + i + 1,
			.score = m_sorted_scores.size() > i ? tr::opt_ref{m_sorted_scores[i]} : std::nullopt
		});
	}

	m_ui.emplace<arrow_widget>(T_GAMEMODE_D, {
		.animation = {{-50, 892.5}, {10, 892.5}, 0.5_s},
		.alignment = tr::valign::BOTTOM,
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_SCOREBOARD && m_savefile.score_categories().size() > 1; },
		.action = [this] { on_gamemode_decrement(); }
	});
	m_ui.emplace<label_widget>(T_GAMEMODE_C, {
		.animation = {BOTTOM_START_POS, {500, 900}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.tooltip_text = [this] { return std::string{m_selected->gamemode.localized_description(m_subsystems->localization)}; },
		.text = [this] { return std::string{m_selected->gamemode.localized_name(m_subsystems->localization)}; }
	});
	m_ui.emplace<arrow_widget>(T_GAMEMODE_I, {
		.animation = {{1050, 892.5}, {990, 892.5}, 0.5_s},
		.alignment = tr::valign::BOTTOM,
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_SCOREBOARD && m_savefile.score_categories().size() > 1; },
		.action = [this] { on_gamemode_increment(); }
	});
	m_ui.emplace<arrow_widget>(T_PAGE_D, {
		.animation = {{-50, 942.5}, {10, 942.5}, 0.5_s},
		.alignment = tr::valign::BOTTOM,
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_SCOREBOARD && m_page > 0; },
		.action = [this] { on_page_decrement(); }
	});
	m_ui.emplace<label_widget>(T_PAGE_C, {
		.animation = {BOTTOM_START_POS, {500, 950}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.text = [this] {
			const int total{std::max(int(m_selected->entries.size()) - 1, 0) / SCORES_PER_PAGE + 1};
			return TR_FMT::format("{}/{}", m_page + 1, total);
		}
	});
	m_ui.emplace<arrow_widget>(T_PAGE_I, {
		.animation = {{1050, 942.5}, {990, 942.5}, 0.5_s},
		.alignment = tr::valign::BOTTOM,
		.type = arrow_type::RIGHT,
		.status = [this] {
			const int last_page{std::max(int(m_selected->entries.size()) - 1, 0) / SCORES_PER_PAGE};
			return m_substate == substate::IN_SCOREBOARD && m_page < last_page;
		},
		.action = [this] { on_page_increment(); }
	});
	// clang-format on
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
	m_next_widgets = std::async(std::launch::async, prepare_next_widgets, m_subsystems->localization,
								(enum score_widget::type)(m_scoreboard), m_sorted_scores, m_page);
}

void scoreboard_state::set_up_exit_animation()
{
	if (m_savefile.score_categories().empty()) {
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

//

void scoreboard_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<scoreboard_selection_state>(m_subsystems, m_game, m_savefile, animate_title::NO);
}

void scoreboard_state::on_gamemode_decrement()
{
	m_substate = substate::SWITCHING_PAGE;
	m_elapsed = 0;
	m_page = 0;
	if (m_selected == m_savefile.score_categories().begin()) {
		m_selected = m_savefile.score_categories().end();
	}
	--m_selected;
	set_up_page_switch_animation();
}

void scoreboard_state::on_gamemode_increment()
{
	m_substate = substate::SWITCHING_PAGE;
	m_elapsed = 0;
	m_page = 0;
	if (++m_selected == m_savefile.score_categories().end()) {
		m_selected = m_savefile.score_categories().begin();
	}
	set_up_page_switch_animation();
}

void scoreboard_state::on_page_decrement()
{
	m_substate = substate::SWITCHING_PAGE;
	m_elapsed = 0;
	--m_page;
	set_up_page_switch_animation();
}

void scoreboard_state::on_page_increment()
{
	m_substate = substate::SWITCHING_PAGE;
	m_elapsed = 0;
	++m_page;
	set_up_page_switch_animation();
}