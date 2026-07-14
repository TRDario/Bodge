///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements game_over_state from state.hpp.                                                                                            //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

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

// Button list.
constexpr std::array BUTTONS{T_SAVE_AND_RESTART, T_RESTART, T_SAVE_AND_QUIT, T_QUIT};

// Selection tree for the game over menu.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_SAVE_AND_RESTART},
	selection_tree_row{T_RESTART},
	selection_tree_row{T_SAVE_AND_QUIT},
	selection_tree_row{T_QUIT},
};

// Shortcut table for the game over menu.
constexpr shortcut_table SHORTCUTS{
	{"Shift+R"_kc, T_SAVE_AND_RESTART}, {"1"_kc, T_SAVE_AND_RESTART},
	{"R"_kc, T_RESTART}, {"2"_kc, T_RESTART},
	{"Shift+Escape"_kc, T_SAVE_AND_QUIT}, {"Shift+Q"_kc, T_SAVE_AND_QUIT}, {"3"_kc, T_SAVE_AND_QUIT},
	{"Escape"_kc, T_QUIT}, {"Q"_kc, T_QUIT}, {"4"_kc, T_QUIT},
};

// Height of the title widget.
constexpr float TITLE_Y{500.0f - (BUTTONS.size() + 3) * 30};

// clang-format on
///////////////////////////////////////////////////////////// GAME OVER STATE /////////////////////////////////////////////////////////////

game_over_state::game_over_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<game> game, savefile savefile, blur_in blur_in)
	: game_menu_state{std::move(subsystems), SELECTION_TREE, SHORTCUTS, std::move(game), std::move(savefile), update_game::YES}
	, m_substate{blur_in == blur_in::YES ? substate::BLURRING_IN : substate::GAME_OVER}
{
	const float result_h{(500 - (BUTTONS.size() - 0.75f) * 30) + 4};
	const float label_h{result_h - m_subsystems->renderer.text_engine.line_skip(font::LANGUAGE, 48) + 14};
	const float best_h{result_h + m_subsystems->renderer.text_engine.line_skip(font::LANGUAGE, 48) - 14};

	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.renderer = m_subsystems->renderer,
		.animation = {{500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s},
		.text = localized_text{m_subsystems->localization, T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_TIME_LABEL, {
		.renderer = m_subsystems->renderer,
		.animation = {{175, label_h}, {275, label_h}, 0.5_s},
		.text = localized_text{m_subsystems->localization, T_TIME_LABEL},
		.font_size = 24,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_TIME, {
		.renderer = m_subsystems->renderer,
		.animation = {{175, result_h}, {275, result_h}, 0.5_s},
		.text = constant_text{format_time(m_game->final_time())},
		.font_size = 64,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_BEST_TIME, {
		.renderer = m_subsystems->renderer,
		.animation = {{175, best_h}, {275, best_h}, 0.5_s},
		.text = best_time_text(),
		.font_size = 24,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_SCORE_LABEL, {
		.renderer = m_subsystems->renderer,
		.animation = {{825, label_h}, {725, label_h}, 0.5_s},
		.text = localized_text{m_subsystems->localization, T_SCORE_LABEL},
		.font_size = 24,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_SCORE, {
		.renderer = m_subsystems->renderer,
		.animation = {{825, result_h}, {725, result_h}, 0.5_s},
		.text = constant_text{format_score(m_game->final_score())},
		.font_size = 64,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_BEST_SCORE, {
		.renderer = m_subsystems->renderer,
		.animation = {{825, best_h}, {725, best_h}, 0.5_s},
		.text = best_score_text(),
		.font_size = 24,
		.color = YELLOW
	});

	struct button_parameters {
		const u16& selected_hue;
		action_command action;
	};
	const std::array<button_parameters, 4> button_parameters{{
		{m_subsystems->settings.primary_hue, [this] { on_save_and_restart(); }},
		{m_subsystems->settings.primary_hue, [this] { on_restart(); }},
		{m_subsystems->settings.primary_hue, [this] { on_save_and_exit(); }},
		{m_subsystems->settings.secondary_hue, [this] { on_exit(); }}
	}};
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS.size() + 3) * 30 + (i + 4) * 60};
		m_ui.emplace<text_button_widget>(BUTTONS[i], {
			.audio = m_subsystems->audio,
			.renderer = m_subsystems->renderer,
			.selected_hue = button_parameters[i].selected_hue,
			.animation = {{500 + offset, y}, {500, y}, 0.5_s},
			.text = localized_text{m_subsystems->localization, BUTTONS[i]},
			.status = [this] { return m_substate == substate::BLURRING_IN || m_substate == substate::GAME_OVER; },
			.action = std::move(button_parameters[i].action),
		});
	}
	// clang-format on
}

//

tr::next_state game_over_state::tick()
{
	const best_results& best_results{m_savefile.best_results(m_game->gamemode())};

	game_menu_state::tick();
	switch (m_substate) {
	case substate::BLURRING_IN:
		if (m_elapsed >= 0.5_s) {
			m_elapsed = 0;
			m_substate = substate::GAME_OVER;
		}
		[[fallthrough]];
	case substate::GAME_OVER:
		if (best_results.time < m_game->final_time()) {
			if (m_elapsed % 0.5_s == 0) {
				m_ui[T_BEST_TIME].hide();
			}
			else if (m_elapsed % 0.5_s == 0.25_s) {
				m_ui[T_BEST_TIME].unhide();
			}
		}
		if (best_results.score < m_game->final_score()) {
			if (m_elapsed % 0.5_s == 0) {
				m_ui[T_BEST_SCORE].hide();
			}
			else if (m_elapsed % 0.5_s == 0.25_s) {
				m_ui[T_BEST_SCORE].unhide();
			}
		}
		return tr::KEEP_STATE;
	case substate::SAVING:
	case substate::RESTARTING:
		return next_state_if_after(0.5_s);
	case substate::QUITTING:
		if (m_elapsed >= 0.5_s) {
			return m_next_state.get();
		}
		else {
			return tr::KEEP_STATE;
		}
	}
}

//

float game_over_state::saturation_factor() const
{
	switch (m_substate) {
	case substate::GAME_OVER:
	case substate::SAVING:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 0.35f;
	case substate::BLURRING_IN:
		return 1 - m_elapsed / 0.5_sf * 0.65f;
	}
}

float game_over_state::blur_strength() const
{
	switch (m_substate) {
	case substate::GAME_OVER:
	case substate::SAVING:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 10;
	case substate::BLURRING_IN:
		return m_elapsed / 0.5_sf * 10;
	}
}

float game_over_state::fade_overlay_opacity() const
{
	return m_substate == substate::RESTARTING || m_substate == substate::QUITTING ? m_elapsed / 0.5_sf : 0;
}

state::cursor_type game_over_state::cursor_type() const
{
	return m_substate == substate::RESTARTING ? cursor_type::transparent : cursor_type::opaque;
}

//

text_command game_over_state::best_time_text() const
{
	const ticks best_time{m_savefile.best_results(m_game->gamemode()).time};

	if (best_time < m_game->final_time()) {
		return localized_text{m_subsystems->localization, "new_personal_best"};
	}
	else {
		return constant_text{TR_FMT::format("{}: {}", m_subsystems->localization["personal_best"], format_time(best_time))};
	}
}

text_command game_over_state::best_score_text() const
{
	const i64 best_score{m_savefile.best_results(m_game->gamemode()).score};

	if (best_score < m_game->final_score()) {
		return localized_text{m_subsystems->localization, "new_personal_best"};
	}
	else {
		return constant_text{TR_FMT::format("{}: {}", m_subsystems->localization["personal_best"], best_score)};
	}
}

//

void game_over_state::set_up_exit_animation()
{
	m_ui[T_TITLE].move_y_and_hide(TITLE_Y - 100, 0.5_s);
	m_ui[T_TIME_LABEL].move_x_and_hide(150, 0.5_s);
	m_ui[T_TIME].move_x_and_hide(150, 0.5_s);
	m_ui[T_BEST_TIME].move_x_and_hide(150, 0.5_s);
	m_ui[T_SCORE_LABEL].move_x_and_hide(850, 0.5_s);
	m_ui[T_SCORE].move_x_and_hide(850, 0.5_s);
	m_ui[T_BEST_SCORE].move_x_and_hide(850, 0.5_s);
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		m_ui[BUTTONS[i]].move_x_and_hide(500 + (i % 2 != 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f), 0.5_s);
	}
}

//

void game_over_state::on_save_and_restart()
{
	m_elapsed = 0;
	m_substate = substate::SAVING;
	set_up_exit_animation();
	m_next_state = make_async<save_score_state>(m_subsystems, m_game, m_savefile, save_screen_flags::RESTARTING);
}

void game_over_state::on_restart()
{
	const score_flags score_flags{false, debug_settings::instance().modified_game_speed()};
	const score_entry score{{}, current_timestamp(), m_game->final_score(), m_game->final_time(), score_flags};

	m_elapsed = 0;
	m_substate = substate::RESTARTING;
	m_savefile.add_score(m_game->gamemode(), score);
	m_savefile.save_to_file();
	set_up_exit_animation();
	// clang-format off
	m_next_state = make_game_state_async<active_game>(
		m_subsystems,
		regular_game_data{},
		std::cref(m_subsystems->input),
		std::ref(m_subsystems->renderer.text_engine),
		m_savefile,
		m_game->gamemode(),
		g_rng.generate<u64>(),
		try_loading_player_skin(m_subsystems->settings.player_skin)
	);
	// clang-format on
}

void game_over_state::on_save_and_exit()
{
	m_elapsed = 0;
	m_substate = substate::SAVING;
	set_up_exit_animation();
	m_next_state = make_async<save_score_state>(m_subsystems, m_game, m_savefile, save_screen_flags::NONE);
}

void game_over_state::on_exit()
{
	const score_flags score_flags{false, debug_settings::instance().modified_game_speed()};
	const score_entry score{{}, current_timestamp(), m_game->final_score(), m_game->final_time(), score_flags};

	m_elapsed = 0;
	m_substate = substate::QUITTING;
	m_savefile.add_score(m_game->gamemode(), score);
	m_savefile.save_to_file();
	set_up_exit_animation();
	m_next_state = make_async<title_state>(m_subsystems);
}