///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements ball_settings_editor_state from state.hpp.                                                                                 //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"gamemode_manager"};
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
constexpr std::array LABELS{
	label_info{T_STARTING_COUNT, "starting_count_tt"},
	label_info{T_MAX_COUNT, "max_count_tt"},
	label_info{T_SPAWN_INTERVAL, "spawn_interval_tt"},
	label_info{T_INITIAL_SIZE, "initial_size_tt"},
	label_info{T_SIZE_STEP, "size_step_tt"},
	label_info{T_INITIAL_VELOCITY, "initial_velocity_tt"},
	label_info{T_VELOCITY_STEP, "velocity_step_tt"},
};

// Right-hand side interactible widgets.
constexpr std::array RIGHT_WIDGETS{
	T_STARTING_COUNT_D, T_STARTING_COUNT_C, T_STARTING_COUNT_I,
	T_MAX_COUNT_D, T_MAX_COUNT_C, T_MAX_COUNT_I,
	T_SPAWN_INTERVAL_D, T_SPAWN_INTERVAL_C, T_SPAWN_INTERVAL_I,
	T_INITIAL_SIZE_D, T_INITIAL_SIZE_C, T_INITIAL_SIZE_I,
	T_SIZE_STEP_D, T_SIZE_STEP_C, T_SIZE_STEP_I,
	T_INITIAL_VELOCITY_D, T_INITIAL_VELOCITY_C, T_INITIAL_VELOCITY_I,
	T_VELOCITY_STEP_D, T_VELOCITY_STEP_C, T_VELOCITY_STEP_I,
};

// Seleciton tree for the ball settings editor menu.
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

// Shortcut table for the ball settings editor menu.
constexpr shortcut_table SHORTCUTS{
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT}, {"1"_kc, T_EXIT},
};

// Starting position for starting count right widgets.
constexpr glm::vec2 STARTING_COUNT_START_POS{1050, 298};
// Starting position for max count right widgets.
constexpr glm::vec2 MAX_COUNT_START_POS{1050, STARTING_COUNT_START_POS.y + 75};
// Starting position for spawn interval right widgets.
constexpr glm::vec2 SPAWN_INTERVAL_START_POS{1050, MAX_COUNT_START_POS.y + 75};
// Starting position for initial size right widgets.
constexpr glm::vec2 INITIAL_SIZE_START_POS{1050, SPAWN_INTERVAL_START_POS.y + 75};
// Starting position for size step right widgets.
constexpr glm::vec2 SIZE_STEP_START_POS{1050, INITIAL_SIZE_START_POS.y + 75};
// Starting position for initial velocity right widgets.
constexpr glm::vec2 INITIAL_VELOCITY_START_POS{1050, SIZE_STEP_START_POS.y + 75};
// Starting position for velocity step right widgets.
constexpr glm::vec2 VELOCITY_STEP_START_POS{1050, INITIAL_VELOCITY_START_POS.y + 75};

// clang-format on
/////////////////////////////////////////////////////// BALL SETTINGS EDITOR STATE ////////////////////////////////////////////////////////

ball_settings_editor_state::ball_settings_editor_state(std::shared_ptr<playerless_game> game, gamemode_editor data, gamemode gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_EDITOR}
	, m_data{std::move(data)}
	, m_pending{std::move(gamemode)}
{
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = TITLE_POS,
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = 0_s,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_SUBTITLE, {
		.animation = {TOP_START_POS, {500, TITLE_POS.y + 64}, 0.5_s},
		.alignment = tr::align::TOP_CENTER,
		.text = localized_text{T_SUBTITLE},
		.font_size = 32
	});
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_D, {
		.animation = {STARTING_COUNT_START_POS, {765, STARTING_COUNT_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.starting_count > 0; },
		.action = [&sc = m_pending.ball.starting_count] { sc = u8(std::max(sc - keymods_choose(1, 5, 10), 1)); }
	});
	m_ui.emplace<numeric_input_widget<u8, 3>>(T_STARTING_COUNT_C,
		tweened_position{STARTING_COUNT_START_POS, {875.5f, STARTING_COUNT_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.ball.starting_count,
		[this] { return m_substate == substate::IN_EDITOR; },
		[&mc = m_pending.ball.max_count](int v) { return u8(std::clamp(v, 0, int(mc))); }
	);
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_I, {
		.animation = {STARTING_COUNT_START_POS, {985, STARTING_COUNT_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.starting_count < m_pending.ball.max_count; },
		.action = [&balls = m_pending.ball] { balls.starting_count = u8(std::min(balls.starting_count + keymods_choose(1, 5, 10), int(balls.max_count))); }
	});
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_D, {
		.animation = {MAX_COUNT_START_POS, {765, MAX_COUNT_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.max_count > std::max(1_u8, m_pending.ball.starting_count); },
		.action = [&balls = m_pending.ball] { balls.max_count = u8(std::max({1, int(balls.starting_count), balls.max_count - keymods_choose(1, 5, 10)})); }
	});
	m_ui.emplace<numeric_input_widget<u8, 3>>(T_MAX_COUNT_C,
		tweened_position{MAX_COUNT_START_POS, {875.5f, MAX_COUNT_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.ball.max_count,
		[this] { return m_substate == substate::IN_EDITOR; },
		[&sc = m_pending.ball.starting_count](int v) { return u8(std::clamp(v, std::max(int(sc), 1), 255)); }
	);
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_I, {
		.animation = {MAX_COUNT_START_POS, {985, MAX_COUNT_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.max_count < 255; },
		.action = [&mc = m_pending.ball.max_count] { mc = u8(std::min(mc + keymods_choose(1, 5, 10), 255)); }
	});
	m_ui.emplace<arrow_widget>(T_SPAWN_INTERVAL_D, {
		.animation = {SPAWN_INTERVAL_START_POS, {765, SPAWN_INTERVAL_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.spawn_interval > 1.0_s; },
		.action = [&si = m_pending.ball.spawn_interval] { si = ticks(std::max(int(si - keymods_choose(0.1_s, 1_s, 10_s)), int(1_s))); }
	});
	m_ui.emplace<interval_input_widget<4>>(T_SPAWN_INTERVAL_C,
		tweened_position{SPAWN_INTERVAL_START_POS, {875.5f, SPAWN_INTERVAL_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.ball.spawn_interval,
		[this] { return m_substate == substate::IN_EDITOR; },
		[](ticks v) { return std::clamp(v, 1_s, 60_s); }
	);
	m_ui.emplace<arrow_widget>(T_SPAWN_INTERVAL_I, {
		.animation = {SPAWN_INTERVAL_START_POS, {985, SPAWN_INTERVAL_START_POS.y}, 0.5_s}, 
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.spawn_interval < 60_s; },
		.action = [&si = m_pending.ball.spawn_interval] { si = std::min(si + keymods_choose(0.1_s, 1_s, 10_s), 60_s); }
	});
	m_ui.emplace<arrow_widget>(T_INITIAL_SIZE_D, {
		.animation = {INITIAL_SIZE_START_POS, {765, INITIAL_SIZE_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_size > 10; },
		.action = [&is = m_pending.ball.initial_size] { is = std::max(is - keymods_choose(1, 5, 10), 10.0f); }
	});
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}">>(T_INITIAL_SIZE_C,
		tweened_position{INITIAL_SIZE_START_POS, {875.5f, INITIAL_SIZE_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.ball.initial_size,
		[this] { return m_substate == substate::IN_EDITOR; },
		[](float v) { return std::clamp(v, 10.0f, 100.0f); }
	);
	m_ui.emplace<arrow_widget>(T_INITIAL_SIZE_I, {
		.animation = {INITIAL_SIZE_START_POS, {985, INITIAL_SIZE_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_size < 100.0f; },
		.action = [&is = m_pending.ball.initial_size] { is = std::min(is + keymods_choose(1, 5, 10), 100.0f); }
	});
	m_ui.emplace<arrow_widget>(T_SIZE_STEP_D, {
		.animation = {SIZE_STEP_START_POS, {765, SIZE_STEP_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.size_step > 0; },
		.action = [&ss = m_pending.ball.size_step] { ss = std::max(ss - keymods_choose(0.1f, 1.0f, 2.5f), 0.0f); }
	});
	m_ui.emplace<numeric_input_widget<float, 4, "{:.1f}">>(T_SIZE_STEP_C,
		tweened_position{SIZE_STEP_START_POS, {875.5f, SIZE_STEP_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.ball.size_step, [this] { return m_substate == substate::IN_EDITOR; },
		[](float v) { return std::clamp(v, 0.0f, 10.0f); }
	);
	m_ui.emplace<arrow_widget>(T_SIZE_STEP_I, {
		.animation = {SIZE_STEP_START_POS, {985, SIZE_STEP_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.size_step < 10.0f; },
		.action = [&ss = m_pending.ball.size_step] { ss = std::min(ss + keymods_choose(0.1f, 1.0f, 2.5f), 10.0f); }
	});
	m_ui.emplace<arrow_widget>(T_INITIAL_VELOCITY_D, {
		.animation = {INITIAL_VELOCITY_START_POS, {765, INITIAL_VELOCITY_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_velocity > 100; },
		.action = [&iv = m_pending.ball.initial_velocity] { iv = std::max(iv - keymods_choose(1, 10, 100), 100.0f); }
	});
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}">>(T_INITIAL_VELOCITY_C,
		tweened_position{INITIAL_VELOCITY_START_POS, {875.5f, INITIAL_VELOCITY_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.ball.initial_velocity,
		[this] { return m_substate == substate::IN_EDITOR; },
		[](float v) { return std::clamp(v, 100.0f, 5000.0f); }
	);
	m_ui.emplace<arrow_widget>(T_INITIAL_VELOCITY_I, {
		.animation = {INITIAL_VELOCITY_START_POS, {985, INITIAL_VELOCITY_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_velocity < 5000.0f; },
		.action = [&iv = m_pending.ball.initial_velocity] { iv = std::min(iv + keymods_choose(1, 10, 100), 5000.0f); }
	});
	m_ui.emplace<arrow_widget>(T_VELOCITY_STEP_D, {
		.animation = {VELOCITY_STEP_START_POS, {765, VELOCITY_STEP_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.velocity_step > 0; },
		.action = [&vs = m_pending.ball.velocity_step] { vs = std::max(vs - keymods_choose(1, 10, 100), 0.0f); }
	});
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}">>(T_VELOCITY_STEP_C,
		tweened_position{VELOCITY_STEP_START_POS, {875.5f, VELOCITY_STEP_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.ball.velocity_step,
		[this] { return m_substate == substate::IN_EDITOR; },
		[](float v) { return std::clamp(v, 0.0f, 1000.0f); }
	);
	m_ui.emplace<arrow_widget>(T_VELOCITY_STEP_I, {
		.animation = {VELOCITY_STEP_START_POS, {985, VELOCITY_STEP_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.ball.velocity_step < 1000.0f; },
		.action = [&vs = m_pending.ball.velocity_step] { vs = std::min(vs + keymods_choose(1, 10, 100), 1000.0f); }
	});

	for (usize i = 0; i < LABELS.size(); ++i) {
		m_ui.emplace<label_widget>(LABELS[i].tag, {
			.animation = {{-50, 298 + i * 75}, {15, 298 + i * 75}, 0.5_s},
			.alignment = tr::align::CENTER_LEFT,
			.tooltip_text = localized_text{LABELS[i].tooltip},
			.text = localized_text{LABELS[i].tag}
		});
	}

	m_ui.emplace<text_button_widget>(T_EXIT,
		tweened_position{BOTTOM_START_POS, {500, 1000}, 0.5_s},
		tr::align::BOTTOM_CENTER,
		0.5_s,
		NO_TOOLTIP,
		localized_text{T_EXIT},
		font::LANGUAGE,
		48,
		[this] { return m_substate == substate::IN_EDITOR; },
		[this] { on_exit(); },
		sound::CANCEL
	);
	// clang-format on
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

//

void ball_settings_editor_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<gamemode_editor_state>(m_game, m_data, m_pending, animate_subtitle::YES);
}