#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr tag T_TITLE{"start_game"};
constexpr tag T_NAME{"name"};
constexpr tag T_AUTHOR{"author"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_BEST_TIME_LABEL{"best_time"};
constexpr tag T_BEST_TIME{"best_time_display"};
constexpr tag T_BEST_SCORE_LABEL{"best_score"};
constexpr tag T_BEST_SCORE{"best_score_display"};
constexpr tag T_PREV{"prev"};
constexpr tag T_NEXT{"next"};
constexpr tag T_START{"start"};
constexpr tag T_EXIT{"exit"};

// Gamemode display widgets.
constexpr std::array<tag, 7> GAMEMODE_WIDGETS{T_NAME, T_AUTHOR, T_DESCRIPTION, T_BEST_TIME_LABEL, T_BEST_TIME, T_BEST_SCORE_LABEL, T_BEST_SCORE};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_START},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::LEFT}, T_PREV},
	{{tr::system::keycode::RIGHT}, T_NEXT},
	{{tr::system::keycode::ENTER}, T_START},
	{{tr::system::keycode::TOP_ROW_1}, T_START},
	{{tr::system::keycode::ESCAPE}, T_EXIT},
	{{tr::system::keycode::TOP_ROW_2}, T_EXIT},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> NAME_MOVE_IN{tween::CUBIC, {500, 275}, {500, 375}, 0.5_s};
constexpr tweener<glm::vec2> AUTHOR_MOVE_IN{tween::CUBIC, {400, 450}, {500, 450}, 0.5_s};
constexpr tweener<glm::vec2> DESCRIPTION_MOVE_IN{tween::CUBIC, {600, 500}, {500, 500}, 0.5_s};
constexpr tweener<glm::vec2> BEST_TIME_LABEL_MOVE_IN{tween::CUBIC, {250, 695}, {250, 595}, 0.5_s};
constexpr tweener<glm::vec2> BEST_SCORE_LABEL_MOVE_IN{tween::CUBIC, {750, 695}, {750, 595}, 0.5_s};
constexpr tweener<glm::vec2> PREV_MOVE_IN{tween::CUBIC, {-50, 500}, {10, 500}, 0.5_s};
constexpr tweener<glm::vec2> NEXT_MOVE_IN{tween::CUBIC, {1050, 500}, {990, 500}, 0.5_s};
constexpr tweener<glm::vec2> START_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

start_game_state::start_game_state(std::unique_ptr<playerless_game>&& game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::ENTERING_START_GAME}
	, m_gamemodes{engine::load_gamemodes()}
	, m_selected{m_gamemodes.begin()}
{
	std::vector<gamemode>::iterator last_selected_it{std::ranges::find(m_gamemodes, engine::scorefile.last_selected)};
	if (last_selected_it != m_gamemodes.end()) {
		m_selected = last_selected_it;
	}

	// TEXT CALLBACKS

	text_callback name_tcb{string_text_callback{std::string{m_selected->name_loc()}}};
	text_callback author_tcb{string_text_callback{TR_FMT::format("{}: {}", engine::loc["by"], m_selected->author)}};
	text_callback description_tcb{string_text_callback{
		std::string{!m_selected->description_loc().empty() ? m_selected->description_loc() : engine::loc["no_description"]}}};
	text_callback best_time_tcb{string_text_callback{timer_text(engine::scorefile.bests(*m_selected).time)}};
	text_callback best_score_tcb{string_text_callback{std::to_string(engine::scorefile.bests(*m_selected).score)}};

	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_START_GAME || m_substate == substate::ENTERING_START_GAME; },
	};
	const status_callback arrow_scb{
		[this] { return m_substate == substate::IN_START_GAME; },
	};

	// ACTION CALLBACKS

	const action_callback prev_acb{
		[this] {
			m_selected = m_selected == m_gamemodes.begin() ? m_selected = m_gamemodes.end() - 1 : std::prev(m_selected);
			m_substate = substate::SWITCHING_GAMEMODE;
			m_timer = 0;
			for (tag tag : GAMEMODE_WIDGETS) {
				widget& widget{m_ui[tag]};
				widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} + glm::vec2{250, 0}, 0.25_s);
				widget.hide(0.25_s);
			}
		},
	};
	const action_callback next_acb{
		[this] {
			if (++m_selected == m_gamemodes.end()) {
				m_selected = m_gamemodes.begin();
			}
			m_substate = substate::SWITCHING_GAMEMODE;
			m_timer = 0;
			for (tag tag : GAMEMODE_WIDGETS) {
				widget& widget{m_ui[tag]};
				widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} - glm::vec2{250, 0}, 0.25_s);
				widget.hide(0.25_s);
			}
		},
	};
	const action_callback start_acb{
		[this] {
			m_substate = substate::ENTERING_GAME;
			m_timer = 0;
			set_up_exit_animation();
			engine::scorefile.last_selected = *m_selected;
			engine::fade_song_out(0.5s);
		},
	};
	const action_callback exit_acb{
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			set_up_exit_animation();
			engine::scorefile.last_selected = *m_selected;
		},
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_NAME, NAME_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(name_tcb),
							   tr::system::ttf_style::NORMAL, 120);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(author_tcb),
							   tr::system::ttf_style::NORMAL, 32);
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(description_tcb),
							   tr::system::ttf_style::ITALIC, 32, "80808080"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_TIME_LABEL, BEST_TIME_LABEL_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   loc_text_callback{T_BEST_TIME_LABEL}, tr::system::ttf_style::NORMAL, 24, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_TIME, BEST_TIME_LABEL_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(best_time_tcb),
							   tr::system::ttf_style::NORMAL, 48, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_SCORE_LABEL, BEST_SCORE_LABEL_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   loc_text_callback{T_BEST_SCORE_LABEL}, tr::system::ttf_style::NORMAL, 24, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_SCORE, BEST_SCORE_LABEL_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(best_score_tcb),
							   tr::system::ttf_style::NORMAL, 48, "FFFF00C0"_rgba8);
	m_ui.emplace<arrow_widget>(T_PREV, PREV_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, arrow_scb, prev_acb);
	m_ui.emplace<arrow_widget>(T_NEXT, NEXT_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, arrow_scb, next_acb);
	m_ui.emplace<text_button_widget>(T_START, START_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_START},
									 font::LANGUAGE, 48, scb, start_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

float start_game_state::fade_overlay_opacity()
{
	return m_substate == substate::ENTERING_GAME ? m_timer / 0.5_sf : 0;
}

std::unique_ptr<tr::state> start_game_state::update(tr::duration)
{
	main_menu_state::update({});
	switch (m_substate) {
	case substate::ENTERING_START_GAME:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_timer = 0;
		}
		return nullptr;
	case substate::IN_START_GAME:
		return nullptr;
	case substate::SWITCHING_GAMEMODE:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_timer = 0;
		}
		else if (m_timer == 0.25_s) {
			std::array<text_callback, GAMEMODE_WIDGETS.size()> new_cbs{
				string_text_callback{std::string{m_selected->name_loc()}},
				string_text_callback{TR_FMT::format("{}: {}", engine::loc["by"], m_selected->author)},
				string_text_callback{
					std::string{!m_selected->description_loc().empty() ? m_selected->description_loc() : engine::loc["no_description"]}},
				loc_text_callback{T_BEST_TIME_LABEL},
				string_text_callback{
					TR_FMT::format("{}:\n{}", engine::loc["best_time"], timer_text(engine::scorefile.bests(*m_selected).time))},
				loc_text_callback{T_BEST_SCORE_LABEL},
				string_text_callback{TR_FMT::format("{}:\n{}", engine::loc["best_score"], engine::scorefile.bests(*m_selected).score)},
			};
			const bool moved_left{glm::vec2{m_ui[GAMEMODE_WIDGETS[0]].pos}.x < 500};
			for (usize i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
				text_widget& widget{m_ui.as<text_widget>(GAMEMODE_WIDGETS[i])};
				const glm::vec2 old_pos{widget.pos};
				widget.text_cb = std::move(new_cbs[i]);
				widget.pos = glm::vec2{moved_left ? old_pos.x + 500 : old_pos.x - 500, old_pos.y};
				widget.pos.change(tween::CUBIC, {moved_left ? old_pos.x + 250 : old_pos.x - 250, old_pos.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? std::make_unique<title_state>(release_game()) : nullptr;
	case substate::ENTERING_GAME:
		return m_timer >= 0.5_s ? std::make_unique<game_state>(std::make_unique<active_game>(*m_selected), game_type::REGULAR, true)
								: nullptr;
	}
}

//

void start_game_state::set_up_exit_animation()
{
	widget& name{m_ui[T_NAME]};
	widget& author{m_ui[T_AUTHOR]};
	widget& description{m_ui[T_DESCRIPTION]};
	widget& best_time_label{m_ui[T_BEST_TIME_LABEL]};
	widget& best_time{m_ui[T_BEST_TIME]};
	widget& best_score_label{m_ui[T_BEST_SCORE_LABEL]};
	widget& best_score{m_ui[T_BEST_SCORE]};
	name.pos.change(tween::CUBIC, glm::vec2{name.pos} - glm::vec2{0, 100}, 0.5_s);
	author.pos.change(tween::CUBIC, glm::vec2{author.pos} + glm::vec2{100, 0}, 0.5_s);
	description.pos.change(tween::CUBIC, glm::vec2{description.pos} - glm::vec2{100, 0}, 0.5_s);
	best_time_label.pos.change(tween::CUBIC, glm::vec2{best_time.pos} + glm::vec2{0, 100}, 0.5_s);
	best_time.pos.change(tween::CUBIC, glm::vec2{best_time.pos} + glm::vec2{0, 100}, 0.5_s);
	best_score_label.pos.change(tween::CUBIC, glm::vec2{best_time.pos} + glm::vec2{0, 100}, 0.5_s);
	best_score.pos.change(tween::CUBIC, glm::vec2{best_score.pos} + glm::vec2{0, 100}, 0.5_s);
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_PREV].pos.change(tween::CUBIC, {-100, 500}, 0.5_s);
	m_ui[T_NEXT].pos.change(tween::CUBIC, {1100, 500}, 0.5_s);
	m_ui[T_START].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui[T_EXIT].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}