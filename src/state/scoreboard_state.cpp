#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

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

constexpr std::array<tag, SCORES_PER_PAGE> SCORE_TAGS{
	T_SCORE_0, T_SCORE_1, T_SCORE_2, T_SCORE_3, T_SCORE_4, T_SCORE_5, T_SCORE_6, T_SCORE_7,
};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::sys::keycode::ESCAPE}, T_EXIT},
	{{tr::sys::keycode::LEFT, tr::sys::keymod::SHIFT}, T_GAMEMODE_D},
	{{tr::sys::keycode::RIGHT, tr::sys::keymod::SHIFT}, T_GAMEMODE_I},
	{{tr::sys::keycode::LEFT}, T_PAGE_D},
	{{tr::sys::keycode::RIGHT}, T_PAGE_I},
};

constexpr tweener<glm::vec2> NO_SCORES_FOUND_MOVE_IN{tween::CUBIC, {600, 500}, {500, 500}, 0.5_s};
constexpr tweener<glm::vec2> GAMEMODE_D_MOVE_IN{tween::CUBIC, {-50, 892.5}, {10, 892.5}, 0.5_s};
constexpr tweener<glm::vec2> GAMEMODE_C_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 900}, 0.5_s};
constexpr tweener<glm::vec2> GAMEMODE_I_MOVE_IN{tween::CUBIC, {1050, 892.5}, {990, 892.5}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_D_MOVE_IN{tween::CUBIC, {-50, 942.5}, {10, 942.5}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_C_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_I_MOVE_IN{tween::CUBIC, {1050, 942.5}, {990, 942.5}, 0.5_s};

// clang-format on

scoreboard_state::scoreboard_state(std::unique_ptr<playerless_game>&& game, scoreboard scoreboard)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate_base::IN_SCOREBOARD | scoreboard}
	, m_page{0}
	, m_selected{engine::scorefile.categories.begin()}
{
	if (!engine::scorefile.categories.empty()) {
		m_sorted_scores = m_selected->scores;
		std::ranges::sort(m_sorted_scores, scoreboard == scoreboard::SCORE ? compare_scores : compare_times);
	}

	// TOOLTIP CALLBACKS

	const text_callback cur_gamemode_ttcb{
		[this] { return std::string{m_selected->gamemode.description_loc()}; },
	};

	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return to_base(m_substate) == substate_base::IN_SCOREBOARD; },
	};
	const status_callback gamemode_change_scb{
		[this] { return to_base(m_substate) == substate_base::IN_SCOREBOARD && engine::scorefile.categories.size() != 1; },
	};
	const status_callback page_d_scb{
		[this] { return to_base(m_substate) == substate_base::IN_SCOREBOARD && m_page > 0; },
	};
	const status_callback page_i_scb{
		[this] {
			return to_base(m_substate) == substate_base::IN_SCOREBOARD &&
				   m_page < (std::max(std::ssize(m_selected->scores) - 1, 0_i64) / SCORES_PER_PAGE);
		},
	};

	// ACTION CALLBACKS

	const action_callback exit_acb{
		[this] {
			m_substate = substate_base::EXITING_TO_SCOREBOARD_SELECTION | to_scoreboard(m_substate);
			m_timer = 0;
			set_up_exit_animation();
		},
	};
	const action_callback gamemode_d_acb{
		[this] {
			m_substate = substate_base::SWITCHING_PAGE | to_scoreboard(m_substate);
			m_timer = 0;
			m_page = 0;
			if (m_selected == engine::scorefile.categories.begin()) {
				m_selected = engine::scorefile.categories.end();
			}
			--m_selected;
			set_up_page_switch_animation();
		},
	};
	const action_callback gamemode_i_acb{
		[this] {
			m_substate = substate_base::SWITCHING_PAGE | to_scoreboard(m_substate);
			m_timer = 0;
			m_page = 0;
			if (++m_selected == engine::scorefile.categories.end()) {
				m_selected = engine::scorefile.categories.begin();
			}
			set_up_page_switch_animation();
		},
	};
	const action_callback page_d_acb{
		[this] {
			m_substate = substate_base::SWITCHING_PAGE | to_scoreboard(m_substate);
			m_timer = 0;
			--m_page;
			set_up_page_switch_animation();
		},
	};
	const action_callback page_i_acb{
		[this] {
			m_substate = substate_base::SWITCHING_PAGE | to_scoreboard(m_substate);
			m_timer = 0;
			++m_page;
			set_up_page_switch_animation();
		},
	};

	// TEXT CALLBACKS

	const text_callback player_info_tcb{
		string_text_callback{TR_FMT::format("{} {}: {}", engine::loc["total_playtime"], engine::scorefile.name,
											format_playtime(engine::scorefile.playtime))},
	};
	const text_callback cur_gamemode_tcb{
		[this] { return std::string{m_selected->gamemode.name_loc()}; },
	};
	const text_callback cur_page_tcb{
		[this] { return TR_FMT::format("{}/{}", m_page + 1, std::max(int(m_selected->scores.size()) - 1, 0) / SCORES_PER_PAGE + 1); },
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_PLAYER_INFO, glm::vec2{500, 64}, tr::align::TOP_CENTER, 0, NO_TOOLTIP, player_info_tcb,
							   tr::sys::ttf_style::NORMAL, 32);
	m_ui.emplace<text_button_widget>(T_EXIT, glm::vec2{500, 1000}, tr::align::BOTTOM_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	if (engine::scorefile.categories.empty()) {
		m_ui.emplace<label_widget>(T_NO_SCORES_FOUND, NO_SCORES_FOUND_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
								   loc_text_callback{T_NO_SCORES_FOUND}, tr::sys::ttf_style::NORMAL, 64, "80808080"_rgba8);
		return;
	}
	for (usize i = 0; i < SCORES_PER_PAGE; ++i) {
		const tweener<glm::vec2> move_in{tween::CUBIC, {i % 2 == 0 ? 400 : 600, 173 + 86 * i}, {500, 173 + 86 * i}, 0.5_s};
		const usize rank{m_page * SCORES_PER_PAGE + i + 1};
		const tr::opt_ref<score_entry> score{m_sorted_scores.size() > i ? tr::opt_ref{m_sorted_scores.begin()[i]} : std::nullopt};
		m_ui.emplace<score_widget>(SCORE_TAGS[i], move_in, tr::align::CENTER, 0.5_s, (enum score_widget::type)(to_scoreboard(m_substate)),
								   rank, score);
	}
	m_ui.emplace<arrow_widget>(T_GAMEMODE_D, GAMEMODE_D_MOVE_IN, tr::align::BOTTOM_LEFT, 0.5_s, false, gamemode_change_scb, gamemode_d_acb);
	m_ui.emplace<label_widget>(T_GAMEMODE_C, GAMEMODE_C_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, cur_gamemode_ttcb, cur_gamemode_tcb,
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_GAMEMODE_I, GAMEMODE_I_MOVE_IN, tr::align::BOTTOM_RIGHT, 0.5_s, true, gamemode_change_scb, gamemode_i_acb);
	m_ui.emplace<arrow_widget>(T_PAGE_D, PAGE_D_MOVE_IN, tr::align::BOTTOM_LEFT, 0.5_s, false, page_d_scb, page_d_acb);
	m_ui.emplace<label_widget>(T_PAGE_C, PAGE_C_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, cur_page_tcb,
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_PAGE_I, PAGE_I_MOVE_IN, tr::align::BOTTOM_RIGHT, 0.5_s, true, page_i_scb, page_i_acb);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> scoreboard_state::update(tr::duration)
{
	main_menu_state::update({});
	switch (to_base(m_substate)) {
	case substate_base::IN_SCOREBOARD:
		return nullptr;
	case substate_base::SWITCHING_PAGE:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate_base::IN_SCOREBOARD | to_scoreboard(m_substate);
		}
		else if (m_timer == 0.25_s) {
			m_sorted_scores = m_selected->scores;
			std::ranges::sort(m_sorted_scores, to_scoreboard(m_substate) == scoreboard::SCORE ? compare_scores : compare_times);
			for (usize i = 0; i < SCORES_PER_PAGE; ++i) {
				const bool nonempty{m_selected->scores.size() > m_page * SCORES_PER_PAGE + i};
				score_widget& widget{m_ui.as<score_widget>(SCORE_TAGS[i])};
				widget.rank = m_page * SCORES_PER_PAGE + i + 1;
				widget.score = nonempty ? tr::opt_ref{m_sorted_scores.begin()[m_page * SCORES_PER_PAGE + i]} : std::nullopt;
				widget.pos = {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y};
				widget.pos.change(tween::CUBIC, {500, glm::vec2{widget.pos}.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
		return nullptr;
	case substate_base::EXITING_TO_SCOREBOARD_SELECTION:
		return m_timer >= 0.5_s ? std::make_unique<scoreboard_selection_state>(release_game(), true) : nullptr;
	}
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

scoreboard_state::substate operator|(const scoreboard_state::substate_base& l, const scoreboard& r)
{
	return scoreboard_state::substate(int(l) | int(r));
}

scoreboard_state::substate_base to_base(scoreboard_state::substate state)
{
	return scoreboard_state::substate_base(int(state) & 0x3);
}

scoreboard to_scoreboard(scoreboard_state::substate state)
{
	return scoreboard(int(state) & 0x4);
}

void scoreboard_state::set_up_page_switch_animation()
{
	for (usize i = 0; i < SCORES_PER_PAGE; i++) {
		widget& widget{m_ui[SCORE_TAGS[i]]};
		widget.pos.change(tween::CUBIC, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
}

void scoreboard_state::set_up_exit_animation()
{
	if (engine::scorefile.categories.empty()) {
		widget& no_scores_found{m_ui[T_NO_SCORES_FOUND]};
		no_scores_found.pos.change(tween::CUBIC, {400, 500}, 0.5_s);
		no_scores_found.hide(0.5_s);
	}
	else {
		for (usize i = 0; i < SCORES_PER_PAGE; i++) {
			widget& widget{m_ui[SCORE_TAGS[i]]};
			widget.pos.change(tween::CUBIC, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.5_s);
			widget.hide(0.5_s);
		}
		widget& gamemode_d{m_ui[T_GAMEMODE_D]};
		widget& gamemode_c{m_ui[T_GAMEMODE_C]};
		widget& gamemode_i{m_ui[T_GAMEMODE_I]};
		widget& page_d{m_ui[T_PAGE_D]};
		widget& page_c{m_ui[T_PAGE_C]};
		widget& page_i{m_ui[T_PAGE_I]};
		gamemode_d.pos.change(tween::CUBIC, {-50, 892.5}, 0.5_s);
		gamemode_d.hide(0.5_s);
		gamemode_c.pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
		gamemode_c.hide(0.5_s);
		gamemode_i.pos.change(tween::CUBIC, {1050, 892.5}, 0.5_s);
		gamemode_i.hide(0.5_s);
		page_d.pos.change(tween::CUBIC, {-50, 942.5}, 0.5_s);
		page_d.hide(0.5_s);
		page_c.pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
		page_c.hide(0.5_s);
		page_i.pos.change(tween::CUBIC, {1050, 942.5}, 0.5_s);
		page_i.hide(0.5_s);
	}
}