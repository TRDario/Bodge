
#include "../../include/state/state.hpp"
#include "../../include/system.hpp"
#include "../../include/ui/widget.hpp"

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

constexpr std::array<label_info, 7> LABELS{{
	{T_STARTING_COUNT, "starting_count_tt"},
	{T_MAX_COUNT, "max_count_tt"},
	{T_SPAWN_INTERVAL, "spawn_interval_tt"},
	{T_INITIAL_SIZE, "initial_size_tt"},
	{T_SIZE_STEP, "size_step_tt"},
	{T_INITIAL_VELOCITY, "initial_velocity_tt"},
	{T_VELOCITY_STEP, "velocity_step_tt"},
}};

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
	{{tr::system::keycode::ESCAPE}, T_EXIT},
	{{tr::system::keycode::TOP_ROW_1}, T_EXIT},
};

constexpr glm::vec2 STARTING_COUNT_START_POS{1050, 298};
constexpr glm::vec2 MAX_COUNT_START_POS{1050, STARTING_COUNT_START_POS.y + 75};
constexpr glm::vec2 SPAWN_INTERVAL_START_POS{1050, MAX_COUNT_START_POS.y + 75};
constexpr glm::vec2 INITIAL_SIZE_START_POS{1050, SPAWN_INTERVAL_START_POS.y + 75};
constexpr glm::vec2 SIZE_STEP_START_POS{1050, INITIAL_SIZE_START_POS.y + 75};
constexpr glm::vec2 INITIAL_VELOCITY_START_POS{1050, SIZE_STEP_START_POS.y + 75};
constexpr glm::vec2 VELOCITY_STEP_START_POS{1050, INITIAL_VELOCITY_START_POS.y + 75};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{TITLE_POS};
constexpr tweener<glm::vec2> SUBTITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, {500, TITLE_POS.y + 64}, 0.5_s};
constexpr tweener<glm::vec2> STARTING_COUNT_D_MOVE_IN{tween::CUBIC, STARTING_COUNT_START_POS, {765, STARTING_COUNT_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> STARTING_COUNT_C_MOVE_IN{tween::CUBIC, STARTING_COUNT_START_POS, {875.5f, STARTING_COUNT_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> STARTING_COUNT_I_MOVE_IN{tween::CUBIC, STARTING_COUNT_START_POS, {985, STARTING_COUNT_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MAX_COUNT_D_MOVE_IN{tween::CUBIC, MAX_COUNT_START_POS, {765, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MAX_COUNT_C_MOVE_IN{tween::CUBIC, MAX_COUNT_START_POS, {875.5f, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MAX_COUNT_I_MOVE_IN{tween::CUBIC, MAX_COUNT_START_POS, {985, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SPAWN_INTERVAL_D_MOVE_IN{tween::CUBIC, SPAWN_INTERVAL_START_POS, {765, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SPAWN_INTERVAL_C_MOVE_IN{tween::CUBIC, SPAWN_INTERVAL_START_POS, {875.5f, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SPAWN_INTERVAL_I_MOVE_IN{tween::CUBIC, SPAWN_INTERVAL_START_POS, {985, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INITIAL_SIZE_D_MOVE_IN{tween::CUBIC, INITIAL_SIZE_START_POS, {765, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INITIAL_SIZE_C_MOVE_IN{tween::CUBIC, INITIAL_SIZE_START_POS, {875.5f, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INITIAL_SIZE_I_MOVE_IN{tween::CUBIC, INITIAL_SIZE_START_POS, {985, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SIZE_STEP_D_MOVE_IN{tween::CUBIC, SIZE_STEP_START_POS, {765, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SIZE_STEP_C_MOVE_IN{tween::CUBIC, SIZE_STEP_START_POS, {875.5f, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SIZE_STEP_I_MOVE_IN{tween::CUBIC, SIZE_STEP_START_POS, {985, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INITIAL_VELOCITY_D_MOVE_IN{tween::CUBIC, INITIAL_VELOCITY_START_POS, {765, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INITIAL_VELOCITY_C_MOVE_IN{tween::CUBIC, INITIAL_VELOCITY_START_POS, {875.5f, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INITIAL_VELOCITY_I_MOVE_IN{tween::CUBIC, INITIAL_VELOCITY_START_POS, {985, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> VELOCITY_STEP_D_MOVE_IN{tween::CUBIC, VELOCITY_STEP_START_POS, {765, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> VELOCITY_STEP_C_MOVE_IN{tween::CUBIC, VELOCITY_STEP_START_POS, {875.5f, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> VELOCITY_STEP_I_MOVE_IN{tween::CUBIC, VELOCITY_STEP_START_POS, {985, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

//////////////////////////////////////////////////////// SPAWN INTERVAL FORMATTER /////////////////////////////////////////////////////////

struct spawn_interval_formatter {
	static void from_string(ticks& out, std::string_view str);
	static std::string to_string(ticks v);
	static std::string to_string(std::string_view str);
};
using tick_input_widget = basic_numeric_input_widget<ticks, 4, spawn_interval_formatter>;

void spawn_interval_formatter::from_string(ticks& out, std::string_view str)
{
	float temp{out / 1.0_sf};
	std::from_chars(str.data(), str.data() + str.size(), temp);
	out = ticks(temp * SECOND_TICKS);
}

std::string spawn_interval_formatter::to_string(ticks v)
{
	return std::format("{:.1f}s", v / 1.0_sf);
}

std::string spawn_interval_formatter::to_string(std::string_view str)
{
	return std::format("{}s", str);
}

/////////////////////////////////////////////////////// BALL SETTINGS EDITOR STATE ////////////////////////////////////////////////////////

ball_settings_editor_state::ball_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_EDITOR}, m_pending{gamemode}
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_EDITOR; },
	};
	const status_callback starting_count_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.starting_count > 0; },
	};
	const status_callback starting_count_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.starting_count < m_pending.ball.max_count; },
	};
	const status_callback max_count_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.max_count > m_pending.ball.starting_count; },
	};
	const status_callback max_count_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.max_count < 255; },
	};
	const status_callback spawn_interval_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.spawn_interval > 1.0_s; },
	};
	const status_callback spawn_interval_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.spawn_interval < 60_s; },
	};
	const status_callback initial_size_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_size > 10; },
	};
	const status_callback initial_size_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_size < 250.0f; },
	};
	const status_callback size_step_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.size_step > 0; },
	};
	const status_callback size_step_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.size_step < 50.0f; },
	};
	const status_callback initial_velocity_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_velocity > 100; },
	};
	const status_callback initial_velocity_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_velocity < 5000.0f; },
	};
	const status_callback velocity_step_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.velocity_step > 0; },
	};
	const status_callback velocity_step_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.velocity_step < 1000.0f; },
	};

	// ACTION CALLBACKS

	const action_callback starting_count_d_acb{
		[&sc = m_pending.ball.starting_count] { sc = std::uint8_t(std::max(sc - engine::keymods_choose(1, 5, 10), 0)); },
	};
	const action_callback starting_count_i_acb{
		[&sc = m_pending.ball.starting_count, &mc = m_pending.ball.max_count] {
			sc = std::uint8_t(std::min(sc + engine::keymods_choose(1, 5, 10), int(mc)));
		},
	};
	const action_callback max_count_d_acb{
		[&sc = m_pending.ball.starting_count, &mc = m_pending.ball.max_count] {
			mc = std::uint8_t(std::max(mc - engine::keymods_choose(1, 5, 10), int(sc)));
		},
	};
	const action_callback max_count_i_acb{
		[&mc = m_pending.ball.max_count] { mc = std::uint8_t(std::min(mc + engine::keymods_choose(1, 5, 10), 255)); },
	};
	const action_callback spawn_interval_d_acb{
		[&si = m_pending.ball.spawn_interval] { si = ticks(std::max(int(si - engine::keymods_choose(0.1_s, 1_s, 10_s)), int(1_s))); },
	};
	const action_callback spawn_interval_i_acb{
		[&si = m_pending.ball.spawn_interval] { si = std::min(si + engine::keymods_choose(0.1_s, 1_s, 10_s), 60_s); },
	};
	const action_callback initial_size_d_acb{
		[&is = m_pending.ball.initial_size] { is = std::max(is - engine::keymods_choose(1, 10, 100), 10.0f); },
	};
	const action_callback initial_size_i_acb{
		[&is = m_pending.ball.initial_size] { is = std::min(is + engine::keymods_choose(1, 10, 100), 250.0f); },
	};
	const action_callback size_step_d_acb{
		[&ss = m_pending.ball.size_step] { ss = std::max(ss - engine::keymods_choose(0.1f, 1.0f, 10.0f), 0.0f); },
	};
	const action_callback size_step_i_acb{
		[&ss = m_pending.ball.size_step] { ss = std::min(ss + engine::keymods_choose(0.1f, 1.0f, 10.0f), 50.0f); },
	};
	const action_callback initial_velocity_d_acb{
		[&iv = m_pending.ball.initial_velocity] { iv = std::max(iv - engine::keymods_choose(1, 10, 100), 100.0f); },
	};
	const action_callback initial_velocity_i_acb{
		[&iv = m_pending.ball.initial_velocity] { iv = std::min(iv + engine::keymods_choose(1, 10, 100), 5000.0f); },
	};
	const action_callback velocity_step_d_acb{
		[&vs = m_pending.ball.velocity_step] { vs = std::max(vs - engine::keymods_choose(1, 10, 100), 0.0f); },
	};
	const action_callback velocity_step_i_acb{
		[&vs = m_pending.ball.velocity_step] { vs = std::min(vs + engine::keymods_choose(1, 10, 100), 1000.0f); },
	};
	const action_callback exit_acb{
		[this] {
			m_substate = substate::EXITING;
			m_timer = 0;
			set_up_exit_animation();
		},
	};

	// VALIDATION CALLBACKS

	const validation_callback<std::uint8_t> starting_count_c_vcb{
		[&max = m_pending.ball.max_count](std::uint8_t v) { return std::min(v, max); },
	};
	const validation_callback<std::uint8_t> max_count_c_vcb{
		[&min = m_pending.ball.starting_count](std::uint8_t v) { return std::max(min, v); },
	};
	const validation_callback<ticks> spawn_interval_c_vcb{
		[](ticks v) { return std::clamp(v, 1_s, 60_s); },
	};
	const validation_callback<float> initial_size_c_vcb{
		[](float v) { return std::clamp(v, 10.0f, 250.0f); },
	};
	const validation_callback<float> size_step_c_vcb{
		[](float v) { return std::clamp(v, 0.0f, 50.0f); },
	};
	const validation_callback<float> initial_velocity_c_vcb{
		[](float v) { return std::clamp(v, 100.0f, 5000.0f); },
	};
	const validation_callback<float> velocity_step_c_vcb{
		[](float v) { return std::clamp(v, 0.0f, 1000.0f); },
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_SUBTITLE, SUBTITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SUBTITLE},
							   tr::system::ttf_style::NORMAL, 32);
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_D, STARTING_COUNT_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, starting_count_d_scb,
							   starting_count_d_acb);
	m_ui.emplace<numeric_input_widget<std::uint8_t, 3, "{}", "{}">>(T_STARTING_COUNT_C, STARTING_COUNT_C_MOVE_IN, tr::align::CENTER, 0.5_s,
																	48, m_ui, m_pending.ball.starting_count, scb, starting_count_c_vcb);
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_I, STARTING_COUNT_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, starting_count_i_scb,
							   starting_count_i_acb);
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_D, MAX_COUNT_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, max_count_d_scb, max_count_d_acb);
	m_ui.emplace<numeric_input_widget<std::uint8_t, 3, "{}", "{}">>(T_MAX_COUNT_C, MAX_COUNT_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
																	m_pending.ball.max_count, scb, max_count_c_vcb);
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_I, MAX_COUNT_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, max_count_i_scb, max_count_i_acb);
	m_ui.emplace<arrow_widget>(T_SPAWN_INTERVAL_D, SPAWN_INTERVAL_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, spawn_interval_d_scb,
							   spawn_interval_d_acb);
	m_ui.emplace<tick_input_widget>(T_SPAWN_INTERVAL_C, SPAWN_INTERVAL_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
									m_pending.ball.spawn_interval, scb, spawn_interval_c_vcb);
	m_ui.emplace<arrow_widget>(T_SPAWN_INTERVAL_I, SPAWN_INTERVAL_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, spawn_interval_i_scb,
							   spawn_interval_i_acb);
	m_ui.emplace<arrow_widget>(T_INITIAL_SIZE_D, INITIAL_SIZE_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, initial_size_d_scb,
							   initial_size_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}", "{}">>(T_INITIAL_SIZE_C, INITIAL_SIZE_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48,
																 m_ui, m_pending.ball.initial_size, scb, initial_size_c_vcb);
	m_ui.emplace<arrow_widget>(T_INITIAL_SIZE_I, INITIAL_SIZE_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, initial_size_i_scb,
							   initial_size_i_acb);
	m_ui.emplace<arrow_widget>(T_SIZE_STEP_D, SIZE_STEP_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, size_step_d_scb, size_step_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.1f}", "{}">>(T_SIZE_STEP_C, SIZE_STEP_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
																 m_pending.ball.size_step, scb, size_step_c_vcb);
	m_ui.emplace<arrow_widget>(T_SIZE_STEP_I, SIZE_STEP_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, size_step_i_scb, size_step_i_acb);
	m_ui.emplace<arrow_widget>(T_INITIAL_VELOCITY_D, INITIAL_VELOCITY_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false,
							   initial_velocity_d_scb, initial_velocity_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}", "{}">>(T_INITIAL_VELOCITY_C, INITIAL_VELOCITY_C_MOVE_IN, tr::align::CENTER, 0.5_s,
																 48, m_ui, m_pending.ball.initial_velocity, scb, initial_velocity_c_vcb);
	m_ui.emplace<arrow_widget>(T_INITIAL_VELOCITY_I, INITIAL_VELOCITY_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true,
							   initial_velocity_i_scb, initial_velocity_i_acb);
	m_ui.emplace<arrow_widget>(T_VELOCITY_STEP_D, VELOCITY_STEP_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, velocity_step_d_scb,
							   velocity_step_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}", "{}">>(T_VELOCITY_STEP_C, VELOCITY_STEP_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48,
																 m_ui, m_pending.ball.velocity_step, scb, velocity_step_c_vcb);
	m_ui.emplace<arrow_widget>(T_VELOCITY_STEP_I, VELOCITY_STEP_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, velocity_step_i_scb,
							   velocity_step_i_acb);
	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const tweener<glm::vec2> move_in{tween::CUBIC, {-50, 298 + i * 75}, {15, 298 + i * 75}, 0.5_s};
		m_ui.emplace<label_widget>(LABELS[i].tag, move_in, tr::align::CENTER_LEFT, 0.5_s, tooltip_loc_text_callback{LABELS[i].tooltip},
								   loc_text_callback{LABELS[i].tag}, tr::system::ttf_style::NORMAL, 48);
	}
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

std::unique_ptr<tr::state> ball_settings_editor_state::update(tr::duration)
{
	main_menu_state::update({});
	switch (m_substate) {
	case substate::IN_EDITOR:
		return nullptr;
	case substate::EXITING:
		return m_timer >= 0.5_s ? std::make_unique<gamemode_designer_state>(release_game(), m_pending, true) : nullptr;
	}
}

//

void ball_settings_editor_state::set_up_exit_animation()
{
	widget& subtitle{m_ui[T_SUBTITLE]};
	widget& exit{m_ui[T_EXIT]};
	subtitle.pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	subtitle.hide(0.5_s);
	for (tag tag : tr::project(LABELS, &label_info::tag)) {
		widget& widget{m_ui[tag]};
		widget.pos.change(tween::CUBIC, {-50, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		widget& widget{m_ui[tag]};
		widget.pos.change(tween::CUBIC, {1050, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	exit.pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	exit.hide(0.5_s);
}