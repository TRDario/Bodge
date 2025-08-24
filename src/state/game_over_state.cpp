#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

//

constexpr tag T_TITLE{"game_over"};
constexpr tag T_TIME{"time"};
constexpr tag T_PERSONAL_BEST{"personal_best"};
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
	{{tr::system::keycode::R, tr::system::keymod::SHIFT}, T_SAVE_AND_RESTART},
	{{tr::system::keycode::TOP_ROW_1}, T_SAVE_AND_RESTART},
	{{tr::system::keycode::R}, T_RESTART},
	{{tr::system::keycode::TOP_ROW_2}, T_RESTART},
	{{tr::system::keycode::ESCAPE, tr::system::keymod::SHIFT}, T_SAVE_AND_QUIT},
	{{tr::system::keycode::Q, tr::system::keymod::SHIFT}, T_SAVE_AND_QUIT},
	{{tr::system::keycode::E, tr::system::keymod::SHIFT}, T_SAVE_AND_QUIT},
	{{tr::system::keycode::TOP_ROW_3}, T_SAVE_AND_QUIT},
	{{tr::system::keycode::ESCAPE}, T_QUIT},
	{{tr::system::keycode::Q}, T_QUIT},
	{{tr::system::keycode::TOP_ROW_4}, T_QUIT},
};

constexpr float TITLE_Y{500.0f - (BUTTONS.size() + 3) * 30};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, glm::vec2{500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};

//

game_over_state::game_over_state(std::unique_ptr<game>&& game, bool blur_in, ticks prev_pb)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), true}
	, m_substate{blur_in ? substate::BLURRING_IN : substate::GAME_OVER}
	, m_prev_pb{prev_pb}
{
	// HEIGHTS AND MOVE-INS

	const float time_h{(500 - (BUTTONS.size() - 0.75f) * 30) -
					   (engine::line_skip(font::LANGUAGE, 48) + 4 + engine::line_skip(font::LANGUAGE, 24)) / 2};
	const float pb_h{time_h + engine::line_skip(font::LANGUAGE, 48) + 4};

	const tweener<glm::vec2> time_move_in{tween::CUBIC, {400, time_h}, {500, time_h}, 0.5_s};
	const tweener<glm::vec2> pb_move_in{tween::CUBIC, {600, pb_h}, {500, pb_h}, 0.5_s};

	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::BLURRING_IN || m_substate == substate::GAME_OVER; },
	};

	// ACTION CALLBACKS

	std::array<action_callback, BUTTONS.size()> action_cbs{
		[this] {
			m_timer = 0;
			m_substate = substate::SAVING_AND_RESTARTING;
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate::RESTARTING;
			engine::scorefile.playtime += m_game->final_time();
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate::SAVING_AND_QUITTING;
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate::QUITTING;
			engine::scorefile.playtime += m_game->final_time();
			set_up_exit_animation();
		},
	};

	// TEXT CALLBACKS

	text_callback pb_tcb;
	if (prev_pb < m_game->final_time()) {
		pb_tcb = loc_text_callback{"new_pb"};
	}
	else {
		pb_tcb = string_text_callback{
			TR_FMT::format("{}: {}", engine::loc["personal_best"], timer_text(engine::scorefile.personal_best(m_game->gamemode())))};
	}

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_TIME, time_move_in, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP,
							   string_text_callback{timer_text(m_game->final_time())}, tr::system::ttf_style::NORMAL, 64, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_PERSONAL_BEST, pb_move_in, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, std::move(pb_tcb),
							   tr::system::ttf_style::NORMAL, 24, "FFFF00C0"_rgba8);
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
		if (m_prev_pb < m_game->final_time()) {
			if (m_timer % 0.5_s == 0) {
				m_ui[T_PERSONAL_BEST].hide();
			}
			else if (m_timer % 0.5_s == 0.25_s) {
				m_ui[T_PERSONAL_BEST].unhide();
			}
		}
		return nullptr;
	case substate::RESTARTING:
		return m_timer >= 0.5_s ? std::make_unique<game_state>(std::make_unique<active_game>(m_game->gamemode()), game_type::REGULAR, true)
								: nullptr;
	case substate::SAVING_AND_RESTARTING:
	case substate::SAVING_AND_QUITTING: {
		if (m_timer >= 0.5_s) {
			const save_screen_flags state_flags{m_substate == substate::SAVING_AND_RESTARTING ? save_screen_flags::RESTARTING
																							  : save_screen_flags::NONE};
			return std::make_unique<save_score_state>(std::move(m_game), m_prev_pb, state_flags);
		}
		else {
			return nullptr;
		}
	}
	case substate::QUITTING:
		return m_timer >= 0.5_s ? std::make_unique<title_state>() : nullptr;
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
	case substate::SAVING_AND_RESTARTING:
	case substate::RESTARTING:
	case substate::SAVING_AND_QUITTING:
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
	case substate::SAVING_AND_RESTARTING:
	case substate::RESTARTING:
	case substate::SAVING_AND_QUITTING:
	case substate::QUITTING:
		return 10;
	case substate::BLURRING_IN:
		return m_timer / 0.5_sf * 10;
	}
}

void game_over_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(tween::CUBIC, {500, TITLE_Y - 100}, 0.5_s);
	widget& time{m_ui[T_TIME]};
	widget& pb{m_ui[T_PERSONAL_BEST]};
	time.pos.change(tween::CUBIC, {400, glm::vec2{time.pos}.y}, 0.5_s);
	pb.pos.change(tween::CUBIC, {600, glm::vec2{pb.pos}.y}, 0.5_s);
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
		widget& widget{m_ui[BUTTONS[i]]};
		widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
}