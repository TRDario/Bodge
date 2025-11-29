
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
	{"Enter"_kc, T_EXIT},
	{"1"_kc, T_EXIT},
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

/////////////////////////////////////////////////////// BALL SETTINGS EDITOR STATE ////////////////////////////////////////////////////////

ball_settings_editor_state::ball_settings_editor_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_EDITOR}, m_pending{gamemode}
{
}

//

void ball_settings_editor_state::set_up_ui()
{
	using enum tr::align;

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 0_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_SUBTITLE, SUBTITLE_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_SUBTITLE}, text_style::NORMAL, 32);
	m_ui.emplace<numeric_arrow_widget<u8, dec, 0, 1, 5, 10>>(T_STARTING_COUNT_D, STARTING_COUNT_D_MOVE_IN, CENTER_LEFT, 0.5_s, interactible,
															 m_pending.ball.starting_count);
	m_ui.emplace<numeric_input_widget<u8, 3, "{}", "{}">>(T_STARTING_COUNT_C, STARTING_COUNT_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
														  m_pending.ball.starting_count, interactible, starting_count_validation_callback);
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_I, STARTING_COUNT_I_MOVE_IN, CENTER_RIGHT, 0.5_s, true, starting_count_i_interactible,
							   on_starting_count_i);
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_D, MAX_COUNT_D_MOVE_IN, CENTER_LEFT, 0.5_s, false, max_count_d_interactible, on_max_count_d);
	m_ui.emplace<numeric_input_widget<u8, 3, "{}", "{}">>(T_MAX_COUNT_C, MAX_COUNT_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
														  m_pending.ball.max_count, interactible, max_count_validation_callback);
	m_ui.emplace<numeric_arrow_widget<u8, inc, 255, 1, 5, 10>>(T_MAX_COUNT_I, MAX_COUNT_I_MOVE_IN, CENTER_RIGHT, 0.5_s, interactible,
															   m_pending.ball.max_count);
	m_ui.emplace<numeric_arrow_widget<ticks, dec, 1_s, 0.1_s, 1_s, 10_s>>(T_SPAWN_INTERVAL_D, SPAWN_INTERVAL_D_MOVE_IN, CENTER_LEFT, 0.5_s,
																		  interactible, m_pending.ball.spawn_interval);
	m_ui.emplace<interval_input_widget<4>>(T_SPAWN_INTERVAL_C, SPAWN_INTERVAL_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
										   m_pending.ball.spawn_interval, interactible, clamp_validation<ticks, 1_s, 60_s>{});
	m_ui.emplace<numeric_arrow_widget<ticks, inc, 60_s, 0.1_s, 1_s, 10_s>>(T_SPAWN_INTERVAL_I, SPAWN_INTERVAL_I_MOVE_IN, CENTER_RIGHT,
																		   0.5_s, interactible, m_pending.ball.spawn_interval);
	m_ui.emplace<numeric_arrow_widget<float, dec, 10.0f, 1.0f, 10.0f, 100.0f>>(T_INITIAL_SIZE_D, INITIAL_SIZE_D_MOVE_IN, CENTER_LEFT, 0.5_s,
																			   interactible, m_pending.ball.initial_size);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}", "{}">>(T_INITIAL_SIZE_C, INITIAL_SIZE_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
																 m_pending.ball.initial_size, interactible,
																 clamp_validation<float, 10.0f, 100.0f>{});
	m_ui.emplace<numeric_arrow_widget<float, inc, 100.0f, 1.0f, 10.0f, 100.0f>>(T_INITIAL_SIZE_I, INITIAL_SIZE_I_MOVE_IN, CENTER_RIGHT,
																				0.5_s, interactible, m_pending.ball.initial_size);
	m_ui.emplace<numeric_arrow_widget<float, dec, 0.0f, 0.1f, 1.0f, 2.5f>>(T_SIZE_STEP_D, SIZE_STEP_D_MOVE_IN, CENTER_LEFT, 0.5_s,
																		   interactible, m_pending.ball.size_step);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.1f}", "{}">>(T_SIZE_STEP_C, SIZE_STEP_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
																 m_pending.ball.size_step, interactible,
																 clamp_validation<float, 0.0f, 10.0f>{});
	m_ui.emplace<numeric_arrow_widget<float, inc, 10.0f, 0.1f, 1.0f, 2.5f>>(T_SIZE_STEP_I, SIZE_STEP_I_MOVE_IN, CENTER_RIGHT, 0.5_s,
																			interactible, m_pending.ball.size_step);
	m_ui.emplace<numeric_arrow_widget<float, dec, 100.0f, 1.0f, 10.0f, 100.0f>>(
		T_INITIAL_VELOCITY_D, INITIAL_VELOCITY_D_MOVE_IN, CENTER_LEFT, 0.5_s, interactible, m_pending.ball.initial_velocity);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}", "{}">>(T_INITIAL_VELOCITY_C, INITIAL_VELOCITY_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
																 m_pending.ball.initial_velocity, interactible,
																 clamp_validation<float, 100.0f, 5000.0f>{});
	m_ui.emplace<numeric_arrow_widget<float, inc, 5000.0f, 1.0f, 10.0f, 100.0f>>(
		T_INITIAL_VELOCITY_I, INITIAL_VELOCITY_I_MOVE_IN, CENTER_RIGHT, 0.5_s, interactible, m_pending.ball.initial_velocity);
	m_ui.emplace<numeric_arrow_widget<float, dec, 0.0f, 1.0f, 10.0f, 100.0f>>(T_VELOCITY_STEP_D, VELOCITY_STEP_D_MOVE_IN, CENTER_LEFT,
																			  0.5_s, interactible, m_pending.ball.velocity_step);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.0f}", "{}">>(T_VELOCITY_STEP_C, VELOCITY_STEP_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
																 m_pending.ball.velocity_step, interactible,
																 clamp_validation<float, 0.0f, 1000.0f>{});
	m_ui.emplace<numeric_arrow_widget<float, inc, 1000.0f, 1.0f, 10.0f, 100.0f>>(T_VELOCITY_STEP_I, VELOCITY_STEP_I_MOVE_IN, CENTER_RIGHT,
																				 0.5_s, interactible, m_pending.ball.velocity_step);
	for (usize i = 0; i < LABELS.size(); ++i) {
		const tweener<glm::vec2> move_in{tween::CUBIC, {-50, 298 + i * 75}, {15, 298 + i * 75}, 0.5_s};
		m_ui.emplace<label_widget>(LABELS[i].tag, move_in, CENTER_LEFT, 0.5_s, tag_tooltip_loc{LABELS[i].tooltip}, tag_loc{LABELS[i].tag},
								   text_style::NORMAL, 48);
	}
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_EXIT}, font::LANGUAGE, 48,
									 interactible, on_exit, sound::CANCEL);
}

next_state ball_settings_editor_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_EDITOR:
		return tr::KEEP_STATE;
	case substate::EXITING:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
	}
}

//

void ball_settings_editor_state::set_up_exit_animation()
{
	widget& subtitle{m_ui[T_SUBTITLE]};
	widget& exit{m_ui[T_EXIT]};
	subtitle.pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	subtitle.hide(0.5_s);
	for (auto [tag, _] : LABELS) {
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

//

bool ball_settings_editor_state::interactible()
{
	const ball_settings_editor_state& self{g_state_machine.get<ball_settings_editor_state>()};

	return self.m_substate == substate::IN_EDITOR;
}

bool ball_settings_editor_state::starting_count_i_interactible()
{
	const ball_settings_editor_state& self{g_state_machine.get<ball_settings_editor_state>()};

	return self.m_substate == substate::IN_EDITOR && self.m_pending.ball.starting_count < self.m_pending.ball.max_count;
}

bool ball_settings_editor_state::max_count_d_interactible()
{
	const ball_settings_editor_state& self{g_state_machine.get<ball_settings_editor_state>()};

	return self.m_substate == substate::IN_EDITOR && self.m_pending.ball.max_count > std::max(1_u8, self.m_pending.ball.starting_count);
};

void ball_settings_editor_state::on_starting_count_i()
{
	ball_settings_editor_state& self{g_state_machine.get<ball_settings_editor_state>()};

	self.m_pending.ball.starting_count =
		u8(std::min(self.m_pending.ball.starting_count + engine::keymods_choose(1, 5, 10), int(self.m_pending.ball.max_count)));
}

void ball_settings_editor_state::on_max_count_d()
{
	ball_settings_editor_state& self{g_state_machine.get<ball_settings_editor_state>()};

	self.m_pending.ball.max_count =
		u8(std::max({1, int(self.m_pending.ball.starting_count), self.m_pending.ball.max_count - engine::keymods_choose(1, 5, 10)}));
}

void ball_settings_editor_state::on_exit()
{
	ball_settings_editor_state& self{g_state_machine.get<ball_settings_editor_state>()};

	self.m_substate = substate::EXITING;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<gamemode_designer_state>(self.m_game, self.m_pending, true);
}

u8 ball_settings_editor_state::starting_count_validation_callback(int value)
{
	const ball_settings_editor_state& self{g_state_machine.get<ball_settings_editor_state>()};

	return u8(std::clamp(value, 0, int(self.m_pending.ball.max_count)));
}

u8 ball_settings_editor_state::max_count_validation_callback(int value)
{
	const ball_settings_editor_state& self{g_state_machine.get<ball_settings_editor_state>()};

	return u8(std::clamp(value, std::max(int(self.m_pending.ball.starting_count), 1), 255));
}