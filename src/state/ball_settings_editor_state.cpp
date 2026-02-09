
#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"gamemode_designer"};
constexpr tag T_SUBTITLE{"ball_settings"};
constexpr tag T_STARTING_COUNT{"starting_count"};
constexpr tag T_STARTING_COUNT_D{"starting_count_d"};
constexpr tag T_STARTING_COUNT_C{"starting_count_c"};
constexpr tag T_STARTING_COUNT_I{"starting_count_i"};
constexpr tag T_MAX_COUNT{"max_count"};
constexpr tag T_MAX_COUNT_D{"max_count_d"};
constexpr tag T_MAX_COUNT_C{"max_count_c"};
constexpr tag T_MAX_COUNT_I{"max_count_i"};
constexpr tag T_SPAWN_INTERVAL{"spawn_interval"};
constexpr tag T_SPAWN_INTERVAL_D{"spawn_interval_d"};
constexpr tag T_SPAWN_INTERVAL_C{"spawn_interval_c"};
constexpr tag T_SPAWN_INTERVAL_I{"spawn_interval_i"};
constexpr tag T_INITIAL_SIZE{"initial_size"};
constexpr tag T_INITIAL_SIZE_D{"initial_size_d"};
constexpr tag T_INITIAL_SIZE_C{"initial_size_c"};
constexpr tag T_INITIAL_SIZE_I{"initial_size_i"};
constexpr tag T_SIZE_STEP{"size_step"};
constexpr tag T_SIZE_STEP_D{"size_step_d"};
constexpr tag T_SIZE_STEP_C{"size_step_c"};
constexpr tag T_SIZE_STEP_I{"size_step_i"};
constexpr tag T_INITIAL_VELOCITY{"initial_velocity"};
constexpr tag T_INITIAL_VELOCITY_D{"initial_velocity_d"};
constexpr tag T_INITIAL_VELOCITY_C{"initial_velocity_c"};
constexpr tag T_INITIAL_VELOCITY_I{"initial_velocity_i"};
constexpr tag T_VELOCITY_STEP{"velocity_step"};
constexpr tag T_VELOCITY_STEP_D{"velocity_step_d"};
constexpr tag T_VELOCITY_STEP_C{"velocity_step_c"};
constexpr tag T_VELOCITY_STEP_I{"velocity_step_i"};
constexpr tag T_EXIT{"exit"};

// Left-hand side labels.
constexpr std::array<label_info, 7> LABELS{{
	{T_STARTING_COUNT, "starting_count_tt"},
	{T_MAX_COUNT, "max_count_tt"},
	{T_SPAWN_INTERVAL, "spawn_interval_tt"},
	{T_INITIAL_SIZE, "initial_size_tt"},
	{T_SIZE_STEP, "size_step_tt"},
	{T_INITIAL_VELOCITY, "initial_velocity_tt"},
	{T_VELOCITY_STEP, "velocity_step_tt"},
}};

// Right-hand side interactible widgets.
constexpr std::array<tag, 21> RIGHT_WIDGETS{
	T_STARTING_COUNT_D, T_STARTING_COUNT_C, T_STARTING_COUNT_I,
	T_MAX_COUNT_D, T_MAX_COUNT_C, T_MAX_COUNT_I,
	T_SPAWN_INTERVAL_D, T_SPAWN_INTERVAL_C, T_SPAWN_INTERVAL_I,
	T_INITIAL_SIZE_D, T_INITIAL_SIZE_C, T_INITIAL_SIZE_I,
	T_SIZE_STEP_D, T_SIZE_STEP_C, T_SIZE_STEP_I,
	T_INITIAL_VELOCITY_D, T_INITIAL_VELOCITY_C, T_INITIAL_VELOCITY_I,
	T_VELOCITY_STEP_D, T_VELOCITY_STEP_C, T_VELOCITY_STEP_I,
};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_STARTING_COUNT_D, T_STARTING_COUNT_C, T_STARTING_COUNT_I},
	selection_tree_row{T_MAX_COUNT_D, T_MAX_COUNT_C, T_MAX_COUNT_I},
	selection_tree_row{T_SPAWN_INTERVAL_D, T_SPAWN_INTERVAL_C, T_SPAWN_INTERVAL_I},
	selection_tree_row{T_INITIAL_SIZE_D, T_INITIAL_SIZE_C, T_INITIAL_SIZE_I},
	selection_tree_row{T_SIZE_STEP_D, T_SIZE_STEP_C, T_SIZE_STEP_I},
	selection_tree_row{T_INITIAL_VELOCITY_D, T_INITIAL_VELOCITY_C, T_INITIAL_VELOCITY_I},
	selection_tree_row{T_VELOCITY_STEP_D, T_VELOCITY_STEP_C, T_VELOCITY_STEP_I},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{"Escape"_kc, T_EXIT},
	{"1"_kc, T_EXIT},
};

constexpr glm::vec2 STARTING_COUNT_START_POS{1050, 298};
constexpr glm::vec2 MAX_COUNT_START_POS{1050, STARTING_COUNT_START_POS.y + 75};
constexpr glm::vec2 SPAWN_INTERVAL_START_POS{1050, MAX_COUNT_START_POS.y + 75};
constexpr glm::vec2 INITIAL_SIZE_START_POS{1050, SPAWN_INTERVAL_START_POS.y + 75};
constexpr glm::vec2 SIZE_STEP_START_POS{1050, INITIAL_SIZE_START_POS.y + 75};
constexpr glm::vec2 INITIAL_VELOCITY_START_POS{1050, SIZE_STEP_START_POS.y + 75};
constexpr glm::vec2 VELOCITY_STEP_START_POS{1050, INITIAL_VELOCITY_START_POS.y + 75};

constexpr tweened_position TITLE_MOVE_IN{TITLE_POS};
constexpr tweened_position SUBTITLE_MOVE_IN{TOP_START_POS, {500, TITLE_POS.y + 64}, 0.5_s};
constexpr tweened_position STARTING_COUNT_D_MOVE_IN{STARTING_COUNT_START_POS, {765, STARTING_COUNT_START_POS.y}, 0.5_s};
constexpr tweened_position STARTING_COUNT_C_MOVE_IN{STARTING_COUNT_START_POS, {875.5f, STARTING_COUNT_START_POS.y}, 0.5_s};
constexpr tweened_position STARTING_COUNT_I_MOVE_IN{STARTING_COUNT_START_POS, {985, STARTING_COUNT_START_POS.y}, 0.5_s};
constexpr tweened_position MAX_COUNT_D_MOVE_IN{MAX_COUNT_START_POS, {765, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr tweened_position MAX_COUNT_C_MOVE_IN{MAX_COUNT_START_POS, {875.5f, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr tweened_position MAX_COUNT_I_MOVE_IN{MAX_COUNT_START_POS, {985, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr tweened_position SPAWN_INTERVAL_D_MOVE_IN{SPAWN_INTERVAL_START_POS, {765, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweened_position SPAWN_INTERVAL_C_MOVE_IN{SPAWN_INTERVAL_START_POS, {875.5f, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweened_position SPAWN_INTERVAL_I_MOVE_IN{SPAWN_INTERVAL_START_POS, {985, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweened_position INITIAL_SIZE_D_MOVE_IN{INITIAL_SIZE_START_POS, {765, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr tweened_position INITIAL_SIZE_C_MOVE_IN{INITIAL_SIZE_START_POS, {875.5f, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr tweened_position INITIAL_SIZE_I_MOVE_IN{INITIAL_SIZE_START_POS, {985, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr tweened_position SIZE_STEP_D_MOVE_IN{SIZE_STEP_START_POS, {765, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr tweened_position SIZE_STEP_C_MOVE_IN{SIZE_STEP_START_POS, {875.5f, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr tweened_position SIZE_STEP_I_MOVE_IN{SIZE_STEP_START_POS, {985, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr tweened_position INITIAL_VELOCITY_D_MOVE_IN{INITIAL_VELOCITY_START_POS, {765, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr tweened_position INITIAL_VELOCITY_C_MOVE_IN{INITIAL_VELOCITY_START_POS, {875.5f, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr tweened_position INITIAL_VELOCITY_I_MOVE_IN{INITIAL_VELOCITY_START_POS, {985, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr tweened_position VELOCITY_STEP_D_MOVE_IN{VELOCITY_STEP_START_POS, {765, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr tweened_position VELOCITY_STEP_C_MOVE_IN{VELOCITY_STEP_START_POS, {875.5f, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr tweened_position VELOCITY_STEP_I_MOVE_IN{VELOCITY_STEP_START_POS, {985, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr tweened_position EXIT_MOVE_IN{BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on
/////////////////////////////////////////////////////// BALL SETTINGS EDITOR STATE ////////////////////////////////////////////////////////

ball_settings_editor_state::ball_settings_editor_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_EDITOR}
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_EDITOR; },
	};
	const status_callback starting_count_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.starting_count > 0; },
	};
	const status_callback starting_count_i_scb{
		[this] {
			return m_substate == substate::IN_EDITOR &&
				   g_scorefile.last_designed.ball.starting_count < g_scorefile.last_designed.ball.max_count;
		},
	};
	const status_callback max_count_d_scb{
		[this] {
			return m_substate == substate::IN_EDITOR &&
				   g_scorefile.last_designed.ball.max_count > std::max(1_u8, g_scorefile.last_designed.ball.starting_count);
		},
	};
	const status_callback max_count_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.max_count < 255; },
	};
	const status_callback spawn_interval_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.spawn_interval > 1.0_s; },
	};
	const status_callback spawn_interval_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.spawn_interval < 60_s; },
	};
	const status_callback initial_size_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.initial_size > 10; },
	};
	const status_callback initial_size_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.initial_size < 100.0f; },
	};
	const status_callback size_step_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.size_step > 0; },
	};
	const status_callback size_step_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.size_step < 10.0f; },
	};
	const status_callback initial_velocity_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.initial_velocity > 100; },
	};
	const status_callback initial_velocity_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.initial_velocity < 5000.0f; },
	};
	const status_callback velocity_step_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.velocity_step > 0; },
	};
	const status_callback velocity_step_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && g_scorefile.last_designed.ball.velocity_step < 1000.0f; },
	};

	// ACTION CALLBACKS

	const action_callback starting_count_d_acb{[] {
		u8& sc{g_scorefile.last_designed.ball.starting_count};
		sc = u8(std::max(sc - keymods_choose(1, 5, 10), 1));
	}};
	const action_callback starting_count_i_acb{[] {
		u8& sc{g_scorefile.last_designed.ball.starting_count};
		sc = u8(std::min(sc + keymods_choose(1, 5, 10), int(g_scorefile.last_designed.ball.max_count)));
	}};
	const action_callback max_count_d_acb{[] {
		u8& mc{g_scorefile.last_designed.ball.max_count};
		mc = u8(std::max({1, int(g_scorefile.last_designed.ball.starting_count), mc - keymods_choose(1, 5, 10)}));
	}};
	const action_callback max_count_i_acb{[] {
		u8& mc{g_scorefile.last_designed.ball.max_count};
		mc = u8(std::min(mc + keymods_choose(1, 5, 10), 255));
	}};
	const action_callback spawn_interval_d_acb{[] {
		ticks& si{g_scorefile.last_designed.ball.spawn_interval};
		si = ticks(std::max(int(si - keymods_choose(0.1_s, 1_s, 10_s)), int(1_s)));
	}};
	const action_callback spawn_interval_i_acb{[] {
		ticks& si{g_scorefile.last_designed.ball.spawn_interval};
		si = std::min(si + keymods_choose(0.1_s, 1_s, 10_s), 60_s);
	}};
	const action_callback initial_size_d_acb{[] {
		float& is{g_scorefile.last_designed.ball.initial_size};
		is = std::max(is - keymods_choose(1, 5, 10), 10.0f);
	}};
	const action_callback initial_size_i_acb{[] {
		float& is{g_scorefile.last_designed.ball.initial_size};
		is = std::min(is + keymods_choose(1, 5, 10), 100.0f);
	}};
	const action_callback size_step_d_acb{[] {
		float& ss{g_scorefile.last_designed.ball.size_step};
		ss = std::max(ss - keymods_choose(0.1f, 1.0f, 2.5f), 0.0f);
	}};
	const action_callback size_step_i_acb{[] {
		float& ss{g_scorefile.last_designed.ball.size_step};
		ss = std::min(ss + keymods_choose(0.1f, 1.0f, 2.5f), 10.0f);
	}};
	const action_callback initial_velocity_d_acb{[] {
		float& iv{g_scorefile.last_designed.ball.initial_velocity};
		iv = std::max(iv - keymods_choose(1, 10, 100), 100.0f);
	}};
	const action_callback initial_velocity_i_acb{[] {
		float& iv{g_scorefile.last_designed.ball.initial_velocity};
		iv = std::min(iv + keymods_choose(1, 10, 100), 5000.0f);
	}};
	const action_callback velocity_step_d_acb{[] {
		float& vs{g_scorefile.last_designed.ball.velocity_step};
		vs = std::max(vs - keymods_choose(1, 10, 100), 0.0f);
	}};
	const action_callback velocity_step_i_acb{[] {
		float& vs{g_scorefile.last_designed.ball.velocity_step};
		vs = std::min(vs + keymods_choose(1, 10, 100), 1000.0f);
	}};
	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING;
		m_elapsed = 0;
		set_up_exit_animation();
		m_next_state = make_async<gamemode_designer_state>(m_game, returning_from_subscreen::YES);
	}};

	// VALIDATION CALLBACKS

	const validation_callback<u8> starting_count_c_vcb{
		[](int v) { return u8(std::clamp(v, 0, int(g_scorefile.last_designed.ball.max_count))); },
	};
	const validation_callback<u8> max_count_c_vcb{
		[](int v) { return u8(std::clamp(v, std::max(int(g_scorefile.last_designed.ball.starting_count), 1), 255)); },
	};
	const validation_callback<ticks> spawn_interval_c_vcb{
		[](ticks v) { return std::clamp(v, 1_s, 60_s); },
	};
	const validation_callback<float> initial_size_c_vcb{
		[](float v) { return std::clamp(v, 10.0f, 100.0f); },
	};
	const validation_callback<float> size_step_c_vcb{
		[](float v) { return std::clamp(v, 0.0f, 10.0f); },
	};
	const validation_callback<float> initial_velocity_c_vcb{
		[](float v) { return std::clamp(v, 100.0f, 5000.0f); },
	};
	const validation_callback<float> velocity_step_c_vcb{
		[](float v) { return std::clamp(v, 0.0f, 1000.0f); },
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_SUBTITLE, SUBTITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SUBTITLE},
							   tr::sys::ttf_style::NORMAL, 32);
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_D, STARTING_COUNT_D_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::LEFT,
							   starting_count_d_scb, starting_count_d_acb);
	m_ui.emplace<numeric_input_widget<u8, 3>>(T_STARTING_COUNT_C, STARTING_COUNT_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
											  g_scorefile.last_designed.ball.starting_count, scb, starting_count_c_vcb);
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_I, STARTING_COUNT_I_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::RIGHT,
							   starting_count_i_scb, starting_count_i_acb);
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_D, MAX_COUNT_D_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::LEFT, max_count_d_scb,
							   max_count_d_acb);
	m_ui.emplace<numeric_input_widget<u8, 3>>(T_MAX_COUNT_C, MAX_COUNT_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
											  g_scorefile.last_designed.ball.max_count, scb, max_count_c_vcb);
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_I, MAX_COUNT_I_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::RIGHT, max_count_i_scb,
							   max_count_i_acb);
	m_ui.emplace<arrow_widget>(T_SPAWN_INTERVAL_D, SPAWN_INTERVAL_D_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::LEFT,
							   spawn_interval_d_scb, spawn_interval_d_acb);
	m_ui.emplace<interval_input_widget<4>>(T_SPAWN_INTERVAL_C, SPAWN_INTERVAL_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
										   g_scorefile.last_designed.ball.spawn_interval, scb, spawn_interval_c_vcb);
	m_ui.emplace<arrow_widget>(T_SPAWN_INTERVAL_I, SPAWN_INTERVAL_I_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::RIGHT,
							   spawn_interval_i_scb, spawn_interval_i_acb);
	m_ui.emplace<arrow_widget>(T_INITIAL_SIZE_D, INITIAL_SIZE_D_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::LEFT, initial_size_d_scb,
							   initial_size_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}">>(T_INITIAL_SIZE_C, INITIAL_SIZE_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
														   g_scorefile.last_designed.ball.initial_size, scb, initial_size_c_vcb);
	m_ui.emplace<arrow_widget>(T_INITIAL_SIZE_I, INITIAL_SIZE_I_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::RIGHT, initial_size_i_scb,
							   initial_size_i_acb);
	m_ui.emplace<arrow_widget>(T_SIZE_STEP_D, SIZE_STEP_D_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::LEFT, size_step_d_scb,
							   size_step_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.1f}">>(T_SIZE_STEP_C, SIZE_STEP_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
														   g_scorefile.last_designed.ball.size_step, scb, size_step_c_vcb);
	m_ui.emplace<arrow_widget>(T_SIZE_STEP_I, SIZE_STEP_I_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::RIGHT, size_step_i_scb,
							   size_step_i_acb);
	m_ui.emplace<arrow_widget>(T_INITIAL_VELOCITY_D, INITIAL_VELOCITY_D_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::LEFT,
							   initial_velocity_d_scb, initial_velocity_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}">>(T_INITIAL_VELOCITY_C, INITIAL_VELOCITY_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48,
														   m_ui, g_scorefile.last_designed.ball.initial_velocity, scb,
														   initial_velocity_c_vcb);
	m_ui.emplace<arrow_widget>(T_INITIAL_VELOCITY_I, INITIAL_VELOCITY_I_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::RIGHT,
							   initial_velocity_i_scb, initial_velocity_i_acb);
	m_ui.emplace<arrow_widget>(T_VELOCITY_STEP_D, VELOCITY_STEP_D_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::LEFT, velocity_step_d_scb,
							   velocity_step_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}">>(T_VELOCITY_STEP_C, VELOCITY_STEP_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
														   g_scorefile.last_designed.ball.velocity_step, scb, velocity_step_c_vcb);
	m_ui.emplace<arrow_widget>(T_VELOCITY_STEP_I, VELOCITY_STEP_I_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::RIGHT,
							   velocity_step_i_scb, velocity_step_i_acb);
	for (usize i = 0; i < LABELS.size(); ++i) {
		const tweened_position move_in{{-50, 298 + i * 75}, {15, 298 + i * 75}, 0.5_s};
		m_ui.emplace<label_widget>(LABELS[i].tag, move_in, tr::align::CENTER_LEFT, 0.5_s, tooltip_loc_text_callback{LABELS[i].tooltip},
								   loc_text_callback{LABELS[i].tag}, tr::sys::ttf_style::NORMAL, 48);
	}
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

tr::next_state ball_settings_editor_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_EDITOR:
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

//

void ball_settings_editor_state::set_up_exit_animation()
{
	m_ui[T_SUBTITLE].move_and_hide(TOP_START_POS, 0.5_s);
	for (const label_info& label : LABELS) {
		m_ui[label.tag].move_x_and_hide(-50, 0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		m_ui[tag].move_x_and_hide(1050, 0.5_s);
	}
	m_ui[T_EXIT].move_and_hide(BOTTOM_START_POS, 0.5_s);
}