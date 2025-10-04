#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

//

constexpr tag T_TITLE{"game_over"};
constexpr tag T_TIME_LABEL{"time"};
constexpr tag T_TIME{"time_display"};
constexpr tag T_BEST_TIME{"best_time"};
constexpr tag T_SCORE_LABEL{"score"};
constexpr tag T_SCORE{"score_display"};
constexpr tag T_BEST_SCORE{"best_score"};
constexpr tag T_SAVE_AND_RESTART{"save_and_restart"};
constexpr tag T_RESTART{"restart"};
constexpr tag T_SAVE_AND_QUIT{"save_and_quit"};
constexpr tag T_QUIT{"quit"};

constexpr std::array<tag, 4> BUTTONS{T_SAVE_AND_RESTART, T_RESTART, T_SAVE_AND_QUIT, T_QUIT};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_SAVE_AND_RESTART},
	selection_tree_row{T_RESTART},
	selection_tree_row{T_SAVE_AND_QUIT},
	selection_tree_row{T_QUIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::sys::keycode::R, tr::sys::keymod::SHIFT}, T_SAVE_AND_RESTART},
	{{tr::sys::keycode::TOP_ROW_1}, T_SAVE_AND_RESTART},
	{{tr::sys::keycode::R}, T_RESTART},
	{{tr::sys::keycode::TOP_ROW_2}, T_RESTART},
	{{tr::sys::keycode::ESCAPE, tr::sys::keymod::SHIFT}, T_SAVE_AND_QUIT},
	{{tr::sys::keycode::Q, tr::sys::keymod::SHIFT}, T_SAVE_AND_QUIT},
	{{tr::sys::keycode::E, tr::sys::keymod::SHIFT}, T_SAVE_AND_QUIT},
	{{tr::sys::keycode::TOP_ROW_3}, T_SAVE_AND_QUIT},
	{{tr::sys::keycode::ESCAPE}, T_QUIT},
	{{tr::sys::keycode::Q}, T_QUIT},
	{{tr::sys::keycode::TOP_ROW_4}, T_QUIT},
};

constexpr float TITLE_Y{500.0f - (BUTTONS.size() + 3) * 30};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, glm::vec2{500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};

//

game_over_state::game_over_state(std::shared_ptr<game> game, bool blur_in)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), true}, m_substate{blur_in ? substate::BLURRING_IN : substate::GAME_OVER}
{
	// HEIGHTS AND MOVE-INS

	const float result_h{(500 - (BUTTONS.size() - 0.75f) * 30) + 4};
	const float label_h{result_h - engine::line_skip(font::LANGUAGE, 48) + 14};
	const float best_h{result_h + engine::line_skip(font::LANGUAGE, 48) - 14};

	const tweener<glm::vec2> time_label_move_in{tween::CUBIC, {175, label_h}, {275, label_h}, 0.5_s};
	const tweener<glm::vec2> time_move_in{tween::CUBIC, {175, result_h}, {275, result_h}, 0.5_s};
	const tweener<glm::vec2> best_time_move_in{tween::CUBIC, {175, best_h}, {275, best_h}, 0.5_s};
	const tweener<glm::vec2> score_label_move_in{tween::CUBIC, {825, label_h}, {725, label_h}, 0.5_s};
	const tweener<glm::vec2> score_move_in{tween::CUBIC, {825, result_h}, {725, result_h}, 0.5_s};
	const tweener<glm::vec2> best_score_move_in{tween::CUBIC, {825, best_h}, {725, best_h}, 0.5_s};

	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::BLURRING_IN || m_substate == substate::GAME_OVER; },
	};

	// ACTION CALLBACKS

	std::array<action_callback, BUTTONS.size()> action_cbs{
		[this] {
			m_timer = 0;
			m_substate = substate::SAVING;
			set_up_exit_animation();
			m_next_state = make_async<save_score_state>(m_game, save_screen_flags::RESTARTING);
		},
		[this] {
			const score_flags score_flags{false, engine::cli_settings.game_speed != 1};
			const score_entry score{{}, current_timestamp(), m_game->final_score(), m_game->final_time(), score_flags};

			m_timer = 0;
			m_substate = substate::RESTARTING;
			engine::scorefile.add_score(m_game->gamemode(), score);
			set_up_exit_animation();
			m_next_state = make_async_game_state<active_game>(game_type::REGULAR, true, m_game->gamemode());
		},
		[this] {
			m_timer = 0;
			m_substate = substate::SAVING;
			set_up_exit_animation();
			m_next_state = make_async<save_score_state>(m_game, save_screen_flags::NONE);
		},
		[this] {
			const score_flags score_flags{false, engine::cli_settings.game_speed != 1};
			const score_entry score{{}, current_timestamp(), m_game->final_score(), m_game->final_time(), score_flags};

			m_timer = 0;
			m_substate = substate::QUITTING;
			engine::scorefile.add_score(m_game->gamemode(), score);
			set_up_exit_animation();
			m_next_state = make_async<title_state>();
		},
	};

	// TEXT CALLBACKS

	const bests& bests{engine::scorefile.bests(m_game->gamemode())};

	text_callback time_tcb{string_text_callback{format_time(m_game->final_time())}};
	text_callback best_time_tcb;
	if (bests.time < m_game->final_time()) {
		best_time_tcb = loc_text_callback{"new_personal_best"};
	}
	else {
		best_time_tcb = string_text_callback{
			TR_FMT::format("{}: {}", engine::loc["personal_best"], format_time(engine::scorefile.bests(m_game->gamemode()).time))};
	}

	text_callback score_tcb{string_text_callback{format_score(m_game->final_score())}};
	text_callback best_score_tcb;
	if (bests.score < m_game->final_score()) {
		best_score_tcb = loc_text_callback{"new_personal_best"};
	}
	else {
		best_score_tcb =
			string_text_callback{TR_FMT::format("{}: {}", engine::loc["personal_best"], engine::scorefile.bests(m_game->gamemode()).score)};
	}

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE}, text_style::NORMAL,
							   64);
	m_ui.emplace<label_widget>(T_TIME_LABEL, time_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TIME_LABEL},
							   text_style::NORMAL, 24, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_TIME, time_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(time_tcb), text_style::NORMAL, 64,
							   "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_TIME, best_time_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(best_time_tcb),
							   text_style::NORMAL, 24, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_SCORE_LABEL, score_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SCORE_LABEL},
							   text_style::NORMAL, 24, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_SCORE, score_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(score_tcb), text_style::NORMAL, 64,
							   "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_SCORE, best_score_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(best_score_tcb),
							   text_style::NORMAL, 24, "FFFF00C0"_rgba8);
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS.size() + 3) * 30 + (i + 4) * 60};
		const tweener<glm::vec2> pos{tween::CUBIC, {500 + offset, y}, {500, y}, 0.5_s};
		m_ui.emplace<text_button_widget>(BUTTONS[i], pos, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{BUTTONS[i]},
										 font::LANGUAGE, 48, scb, std::move(action_cbs[i]), sound::CONFIRM);
	}
}

//

std::unique_ptr<tr::state> game_over_state::update(tr::duration)
{
	game_menu_state::update({});
	switch (m_substate) {
	case substate::BLURRING_IN:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate::GAME_OVER;
		}
		[[fallthrough]];
	case substate::GAME_OVER:
		if (engine::scorefile.bests(m_game->gamemode()).time < m_game->final_time()) {
			if (m_timer % 0.5_s == 0) {
				m_ui[T_BEST_TIME].hide();
			}
			else if (m_timer % 0.5_s == 0.25_s) {
				m_ui[T_BEST_TIME].unhide();
			}
		}
		if (engine::scorefile.bests(m_game->gamemode()).score < m_game->final_score()) {
			if (m_timer % 0.5_s == 0) {
				m_ui[T_BEST_SCORE].hide();
			}
			else if (m_timer % 0.5_s == 0.25_s) {
				m_ui[T_BEST_SCORE].unhide();
			}
		}
		return nullptr;
	case substate::SAVING:
	case substate::RESTARTING:
		return m_timer >= 0.5_s ? m_next_state.get() : nullptr;
	case substate::QUITTING:
		if (m_timer >= 0.5_s) {
			engine::play_song("menu", 1.0s);
			return m_next_state.get();
		}
		else {
			return nullptr;
		}
	}
}

//

float game_over_state::fade_overlay_opacity()
{
	return m_substate == substate::RESTARTING || m_substate == substate::QUITTING ? m_timer / 0.5_sf : 0;
}

float game_over_state::saturation_factor()
{
	switch (m_substate) {
	case substate::GAME_OVER:
	case substate::SAVING:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 0.35f;
	case substate::BLURRING_IN:
		return 1 - m_timer / 0.5_sf * 0.65f;
	}
}

float game_over_state::blur_strength()
{
	switch (m_substate) {
	case substate::GAME_OVER:
	case substate::SAVING:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 10;
	case substate::BLURRING_IN:
		return m_timer / 0.5_sf * 10;
	}
}

void game_over_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(tween::CUBIC, {500, TITLE_Y - 100}, 0.5_s);
	widget& time_label{m_ui[T_TIME_LABEL]};
	widget& time{m_ui[T_TIME]};
	widget& best_time{m_ui[T_BEST_TIME]};
	widget& score_label{m_ui[T_SCORE_LABEL]};
	widget& score{m_ui[T_SCORE]};
	widget& best_score{m_ui[T_BEST_SCORE]};
	time_label.pos.change(tween::CUBIC, {150, glm::vec2{time_label.pos}.y}, 0.5_s);
	time.pos.change(tween::CUBIC, {150, glm::vec2{time.pos}.y}, 0.5_s);
	best_time.pos.change(tween::CUBIC, {150, glm::vec2{best_time.pos}.y}, 0.5_s);
	score_label.pos.change(tween::CUBIC, {850, glm::vec2{score_label.pos}.y}, 0.5_s);
	score.pos.change(tween::CUBIC, {850, glm::vec2{score.pos}.y}, 0.5_s);
	best_score.pos.change(tween::CUBIC, {850, glm::vec2{best_score.pos}.y}, 0.5_s);
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
		widget& widget{m_ui[BUTTONS[i]]};
		widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
}