#include "../../include/state/ball_settings_editor_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/system.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

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

// Left-aligned labels.
constexpr std::array<label, 7> LABELS{{
	{T_STARTING_COUNT, "starting_count_tt"},
	{T_MAX_COUNT, "max_count_tt"},
	{T_SPAWN_INTERVAL, "spawn_interval_tt"},
	{T_INITIAL_SIZE, "initial_size_tt"},
	{T_SIZE_STEP, "size_step_tt"},
	{T_INITIAL_VELOCITY, "initial_velocity_tt"},
	{T_VELOCITY_STEP, "velocity_step_tt"},
}};
// Right-aligned widgets.
constexpr std::array<tag, 21> RIGHT_WIDGETS{
	T_STARTING_COUNT_D, T_STARTING_COUNT_C, T_STARTING_COUNT_I, T_MAX_COUNT_D,        T_MAX_COUNT_C,        T_MAX_COUNT_I,
	T_SPAWN_INTERVAL_D, T_SPAWN_INTERVAL_C, T_SPAWN_INTERVAL_I, T_INITIAL_SIZE_D,     T_INITIAL_SIZE_C,     T_INITIAL_SIZE_I,
	T_SIZE_STEP_D,      T_SIZE_STEP_C,      T_SIZE_STEP_I,      T_INITIAL_VELOCITY_D, T_INITIAL_VELOCITY_C, T_INITIAL_VELOCITY_I,
	T_VELOCITY_STEP_D,  T_VELOCITY_STEP_C,  T_VELOCITY_STEP_I,
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::ESCAPE}, T_EXIT},
	{{tr::system::keycode::TOP_ROW_1}, T_EXIT},
};

// Starting position of the starting count widgets.
constexpr glm::vec2 STARTING_COUNT_START_POS{1050, 298};
// Starting position of the max count widgets.
constexpr glm::vec2 MAX_COUNT_START_POS{1050, STARTING_COUNT_START_POS.y + 75};
// Starting position of the spawn interval widgets.
constexpr glm::vec2 SPAWN_INTERVAL_START_POS{1050, MAX_COUNT_START_POS.y + 75};
// Starting position of the initial size widgets.
constexpr glm::vec2 INITIAL_SIZE_START_POS{1050, SPAWN_INTERVAL_START_POS.y + 75};
// Starting position of the size step widgets.
constexpr glm::vec2 SIZE_STEP_START_POS{1050, INITIAL_SIZE_START_POS.y + 75};
// Starting position of the initial velocity widgets.
constexpr glm::vec2 INITIAL_VELOCITY_START_POS{1050, SIZE_STEP_START_POS.y + 75};
// Starting position of the velocity step widgets.
constexpr glm::vec2 VELOCITY_STEP_START_POS{1050, INITIAL_VELOCITY_START_POS.y + 75};

constexpr interpolator<glm::vec2> TITLE_MOVE_IN{TITLE_POS};
constexpr interpolator<glm::vec2> SUBTITLE_MOVE_IN{interp::CUBIC, TOP_START_POS, {500, TITLE_POS.y + 64}, 0.5_s};
constexpr interpolator<glm::vec2> STARTING_COUNT_D_MOVE_IN{
	interpolator<glm::vec2>{interp::CUBIC, STARTING_COUNT_START_POS, {765, STARTING_COUNT_START_POS.y}, 0.5_s}};
constexpr interpolator<glm::vec2> STARTING_COUNT_C_MOVE_IN{
	interpolator<glm::vec2>{interp::CUBIC, STARTING_COUNT_START_POS, {875.5f, STARTING_COUNT_START_POS.y}, 0.5_s}};
constexpr interpolator<glm::vec2> STARTING_COUNT_I_MOVE_IN{
	interpolator<glm::vec2>{interp::CUBIC, STARTING_COUNT_START_POS, {985, STARTING_COUNT_START_POS.y}, 0.5_s}};
constexpr interpolator<glm::vec2> MAX_COUNT_D_MOVE_IN{interp::CUBIC, MAX_COUNT_START_POS, {765, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> MAX_COUNT_C_MOVE_IN{interp::CUBIC, MAX_COUNT_START_POS, {875.5f, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> MAX_COUNT_I_MOVE_IN{interp::CUBIC, MAX_COUNT_START_POS, {985, MAX_COUNT_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> SPAWN_INTERVAL_D_MOVE_IN{
	interp::CUBIC, SPAWN_INTERVAL_START_POS, {765, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> SPAWN_INTERVAL_C_MOVE_IN{
	interp::CUBIC, SPAWN_INTERVAL_START_POS, {875.5f, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> SPAWN_INTERVAL_I_MOVE_IN{
	interp::CUBIC, SPAWN_INTERVAL_START_POS, {985, SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INITIAL_SIZE_D_MOVE_IN{interp::CUBIC, INITIAL_SIZE_START_POS, {765, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INITIAL_SIZE_C_MOVE_IN{interp::CUBIC, INITIAL_SIZE_START_POS, {875.5f, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INITIAL_SIZE_I_MOVE_IN{interp::CUBIC, INITIAL_SIZE_START_POS, {985, INITIAL_SIZE_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> SIZE_STEP_D_MOVE_IN{interp::CUBIC, SIZE_STEP_START_POS, {765, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> SIZE_STEP_C_MOVE_IN{interp::CUBIC, SIZE_STEP_START_POS, {875.5f, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> SIZE_STEP_I_MOVE_IN{interp::CUBIC, SIZE_STEP_START_POS, {985, SIZE_STEP_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INITIAL_VELOCITY_D_MOVE_IN{
	interp::CUBIC, INITIAL_VELOCITY_START_POS, {765, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INITIAL_VELOCITY_C_MOVE_IN{
	interp::CUBIC, INITIAL_VELOCITY_START_POS, {875.5f, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INITIAL_VELOCITY_I_MOVE_IN{
	interp::CUBIC, INITIAL_VELOCITY_START_POS, {985, INITIAL_VELOCITY_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> VELOCITY_STEP_D_MOVE_IN{interp::CUBIC, VELOCITY_STEP_START_POS, {765, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> VELOCITY_STEP_C_MOVE_IN{
	interp::CUBIC, VELOCITY_STEP_START_POS, {875.5f, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> VELOCITY_STEP_I_MOVE_IN{interp::CUBIC, VELOCITY_STEP_START_POS, {985, VELOCITY_STEP_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> EXIT_MOVE_IN{interp::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

ball_settings_editor_state::ball_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode)
	: m_substate{substate::IN_EDITOR}, m_timer{0}, m_ui{SHORTCUTS}, m_background_game{std::move(game)}, m_pending{gamemode}
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

	const action_callback starting_count_d_acb{[&starting_count = m_pending.ball.starting_count] {
		starting_count = static_cast<std::uint8_t>(std::max(starting_count - engine::keymods_choose(1, 5, 10), 0));
	}};
	const action_callback starting_count_i_acb{[&ball = m_pending.ball] {
		ball.starting_count =
			static_cast<std::uint8_t>(std::min(ball.starting_count + engine::keymods_choose(1, 5, 10), static_cast<int>(ball.max_count)));
	}};
	const action_callback max_count_d_acb{[&ball = m_pending.ball] {
		ball.max_count =
			static_cast<std::uint8_t>(std::max(ball.max_count - engine::keymods_choose(1, 5, 10), static_cast<int>(ball.starting_count)));
	}};
	const action_callback max_count_i_acb{[&max_count = m_pending.ball.max_count] {
		max_count = static_cast<std::uint8_t>(std::min(max_count + engine::keymods_choose(1, 5, 10), 255));
	}};
	const action_callback spawn_interval_d_acb{[&spawn_interval = m_pending.ball.spawn_interval] {
		spawn_interval = static_cast<ticks>(
			std::max(static_cast<int>(spawn_interval - engine::keymods_choose(0.1_s, 1_s, 10_s)), static_cast<int>(1_s)));
	}};
	const action_callback spawn_interval_i_acb{[&spawn_interval = m_pending.ball.spawn_interval] {
		spawn_interval = std::min(spawn_interval + engine::keymods_choose(0.1_s, 1_s, 10_s), 60_s);
	}};
	const action_callback initial_size_d_acb{[&initial_size = m_pending.ball.initial_size] {
		initial_size = std::max(initial_size - engine::keymods_choose(1, 10, 100), 10.0f);
	}};
	const action_callback initial_size_i_acb{[&initial_size = m_pending.ball.initial_size] {
		initial_size = std::min(initial_size + engine::keymods_choose(1, 10, 100), 250.0f);
	}};
	const action_callback size_step_d_acb{
		[&size_step = m_pending.ball.size_step] { size_step = std::max(size_step - engine::keymods_choose(0.1f, 1.0f, 10.0f), 0.0f); }};
	const action_callback size_step_i_acb{
		[&size_step = m_pending.ball.size_step] { size_step = std::min(size_step + engine::keymods_choose(0.1f, 1.0f, 10.0f), 50.0f); }};
	const action_callback initial_velocity_d_acb{[&initial_velocity = m_pending.ball.initial_velocity] {
		initial_velocity = std::max(initial_velocity - engine::keymods_choose(1, 10, 100), 100.0f);
	}};
	const action_callback initial_velocity_i_acb{[&initial_velocity = m_pending.ball.initial_velocity] {
		initial_velocity = std::min(initial_velocity + engine::keymods_choose(1, 10, 100), 5000.0f);
	}};
	const action_callback velocity_step_d_acb{[&velocity_step = m_pending.ball.velocity_step] {
		velocity_step = std::max(velocity_step - engine::keymods_choose(1, 10, 100), 0.0f);
	}};
	const action_callback velocity_step_i_acb{[&velocity_step = m_pending.ball.velocity_step] {
		velocity_step = std::min(velocity_step + engine::keymods_choose(1, 10, 100), 1000.0f);
	}};
	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING;
		m_timer = 0;
		set_up_exit_animation();
	}};

	// TEXT CALLBACKS

	const text_callback cur_starting_count_tcb{[this] { return std::to_string(m_pending.ball.starting_count); }};
	const text_callback cur_max_count_tcb{[this] { return std::to_string(m_pending.ball.max_count); }};
	const text_callback cur_spawn_interval_tcb{[this] { return std::format("{:.1f}s", m_pending.ball.spawn_interval / 1_sf); }};
	const text_callback cur_initial_size_tcb{[this] { return std::format("{:.0f}", m_pending.ball.initial_size); }};
	const text_callback cur_size_step_tcb{[this] { return std::format("{:.1f}", m_pending.ball.size_step); }};
	const text_callback cur_initial_velocity_tcb{[this] { return std::format("{:.0f}", m_pending.ball.initial_velocity); }};
	const text_callback cur_velocity_step_tcb{[this] { return std::format("{:.0f}", m_pending.ball.velocity_step); }};

	//

	m_ui.emplace<text_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0_s, font::LANGUAGE, tr::system::ttf_style::NORMAL, 64,
							  loc_text_callback{T_TITLE});
	m_ui.emplace<text_widget>(T_SUBTITLE, SUBTITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, font::LANGUAGE, tr::system::ttf_style::NORMAL, 32,
							  loc_text_callback{T_SUBTITLE});
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_D, STARTING_COUNT_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, starting_count_d_scb,
							   starting_count_d_acb);
	m_ui.emplace<text_widget>(T_STARTING_COUNT_C, STARTING_COUNT_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE,
							  tr::system::ttf_style::NORMAL, 48, cur_starting_count_tcb);
	m_ui.emplace<arrow_widget>(T_STARTING_COUNT_I, STARTING_COUNT_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, starting_count_i_scb,
							   starting_count_i_acb);
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_D, MAX_COUNT_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, max_count_d_scb, max_count_d_acb);
	m_ui.emplace<text_widget>(T_MAX_COUNT_C, MAX_COUNT_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE, tr::system::ttf_style::NORMAL,
							  48, cur_max_count_tcb);
	m_ui.emplace<arrow_widget>(T_MAX_COUNT_I, MAX_COUNT_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, max_count_i_scb, max_count_i_acb);
	m_ui.emplace<arrow_widget>(T_SPAWN_INTERVAL_D, SPAWN_INTERVAL_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, spawn_interval_d_scb,
							   spawn_interval_d_acb);
	m_ui.emplace<text_widget>(T_SPAWN_INTERVAL_C, SPAWN_INTERVAL_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE,
							  tr::system::ttf_style::NORMAL, 48, cur_spawn_interval_tcb);
	m_ui.emplace<arrow_widget>(T_SPAWN_INTERVAL_I, SPAWN_INTERVAL_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, spawn_interval_i_scb,
							   spawn_interval_i_acb);
	m_ui.emplace<arrow_widget>(T_INITIAL_SIZE_D, INITIAL_SIZE_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, initial_size_d_scb,
							   initial_size_d_acb);
	m_ui.emplace<text_widget>(T_INITIAL_SIZE_C, INITIAL_SIZE_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE,
							  tr::system::ttf_style::NORMAL, 48, cur_initial_size_tcb);
	m_ui.emplace<arrow_widget>(T_INITIAL_SIZE_I, INITIAL_SIZE_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, initial_size_i_scb,
							   initial_size_i_acb);
	m_ui.emplace<arrow_widget>(T_SIZE_STEP_D, SIZE_STEP_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, size_step_d_scb, size_step_d_acb);
	m_ui.emplace<text_widget>(T_SIZE_STEP_C, SIZE_STEP_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE, tr::system::ttf_style::NORMAL,
							  48, cur_size_step_tcb);
	m_ui.emplace<arrow_widget>(T_SIZE_STEP_I, SIZE_STEP_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, size_step_i_scb, size_step_i_acb);
	m_ui.emplace<arrow_widget>(T_INITIAL_VELOCITY_D, INITIAL_VELOCITY_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false,
							   initial_velocity_d_scb, initial_velocity_d_acb);
	m_ui.emplace<text_widget>(T_INITIAL_VELOCITY_C, INITIAL_VELOCITY_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE,
							  tr::system::ttf_style::NORMAL, 48, cur_initial_velocity_tcb);
	m_ui.emplace<arrow_widget>(T_INITIAL_VELOCITY_I, INITIAL_VELOCITY_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true,
							   initial_velocity_i_scb, initial_velocity_i_acb);
	m_ui.emplace<arrow_widget>(T_VELOCITY_STEP_D, VELOCITY_STEP_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, velocity_step_d_scb,
							   velocity_step_d_acb);
	m_ui.emplace<text_widget>(T_VELOCITY_STEP_C, VELOCITY_STEP_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE,
							  tr::system::ttf_style::NORMAL, 48, cur_velocity_step_tcb);
	m_ui.emplace<arrow_widget>(T_VELOCITY_STEP_I, VELOCITY_STEP_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, velocity_step_i_scb,
							   velocity_step_i_acb);
	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const interpolator<glm::vec2> move_in{interp::CUBIC, {-50, 298 + i * 75}, {15, 298 + i * 75}, 0.5_s};
		m_ui.emplace<text_widget>(LABELS[i].tag, move_in, tr::align::CENTER_LEFT, 0.5_s, LABELS[i].tooltip, font::LANGUAGE,
								  tr::system::ttf_style::NORMAL, 48, loc_text_callback{LABELS[i].tag});
	}
	m_ui.emplace<clickable_text_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, font::LANGUAGE, 48,
										loc_text_callback{T_EXIT}, scb, exit_acb, NO_TOOLTIP, sound::CANCEL);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> ball_settings_editor_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> ball_settings_editor_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

	switch (m_substate) {
	case substate::IN_EDITOR:
		return nullptr;
	case substate::EXITING:
		return m_timer >= 0.5_s ? std::make_unique<gamemode_designer_state>(std::move(m_background_game), m_pending, true) : nullptr;
	}
}

void ball_settings_editor_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void ball_settings_editor_state::set_up_exit_animation()
{
	widget& subtitle{m_ui[T_SUBTITLE]};
	widget& exit{m_ui[T_EXIT]};
	subtitle.pos.change(interp::CUBIC, TOP_START_POS, 0.5_s);
	subtitle.hide(0.5_s);
	for (tag tag : tr::project(LABELS, &label::tag)) {
		widget& widget{m_ui[tag]};
		widget.pos.change(interp::CUBIC, {-50, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		widget& widget{m_ui[tag]};
		widget.pos.change(interp::CUBIC, {1050, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	exit.pos.change(interp::CUBIC, BOTTOM_START_POS, 0.5_s);
	exit.hide(0.5_s);
}