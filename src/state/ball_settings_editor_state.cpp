#include "../../include/state/ball_settings_editor_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/system.hpp"

// Left-aligned labels.
constexpr std::array<label, 7> LABELS{{
	{"starting_count", "starting_count_tt"},
	{"max_count", "max_count_tt"},
	{"spawn_interval", "spawn_interval_tt"},
	{"initial_size", "initial_size_tt"},
	{"size_step", "size_step_tt"},
	{"initial_velocity", "initial_velocity_tt"},
	{"velocity_step", "velocity_step_tt"},
}};
constexpr std::array<const char*, 21> RIGHT_WIDGETS{
	"starting_count_dec",   "cur_starting_count",   "starting_count_inc",   "max_count_dec",      "cur_max_count",
	"max_count_inc",        "spawn_interval_dec",   "cur_spawn_interval",   "spawn_interval_inc", "initial_size_dec",
	"cur_initial_size",     "initial_size_inc",     "size_step_dec",        "cur_size_step",      "size_step_inc",
	"initial_velocity_dec", "cur_initial_velocity", "initial_velocity_inc", "velocity_step_dec",  "cur_velocity_step",
	"velocity_step_inc",
};
// Shortcuts of the exit button.
constexpr std::initializer_list<tr::system::key_chord> EXIT_SHORTCUTS{
	{tr::system::keycode::C},      {tr::system::keycode::Q},         {tr::system::keycode::E},
	{tr::system::keycode::ESCAPE}, {tr::system::keycode::TOP_ROW_2},
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

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

ball_settings_editor_state::ball_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode)
	: m_substate{substate::IN_EDITOR}, m_timer{0}, m_background_game{std::move(game)}, m_pending{gamemode}
{
	// STATUS CALLBACKS

	const status_callback status_cb{
		[this] { return m_substate == substate::IN_EDITOR; },
	};
	const status_callback starting_count_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.starting_count > 0; },
	};
	const status_callback starting_count_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.starting_count < m_pending.ball.max_count; },
	};
	const status_callback max_count_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.max_count > m_pending.ball.starting_count; },
	};
	const status_callback max_count_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.max_count < 255; },
	};
	const status_callback spawn_interval_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.spawn_interval > 1.0_s; },
	};
	const status_callback spawn_interval_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.spawn_interval < 60_s; },
	};
	const status_callback initial_size_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_size > 10; },
	};
	const status_callback initial_size_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_size < 250.0f; },
	};
	const status_callback size_step_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.size_step > 0; },
	};
	const status_callback size_step_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.size_step < 50.0f; },
	};
	const status_callback initial_velocity_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_velocity > 100; },
	};
	const status_callback initial_velocity_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.initial_velocity < 5000.0f; },
	};
	const status_callback velocity_step_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.velocity_step > 0; },
	};
	const status_callback velocity_step_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.ball.velocity_step < 1000.0f; },
	};

	// ACTION CALLBACKS

	const action_callback starting_count_dec_action_cb{[&starting_count = m_pending.ball.starting_count] {
		starting_count = static_cast<std::uint8_t>(std::max(starting_count - engine::keymods_choose(1, 5, 10), 0));
	}};
	const action_callback starting_count_inc_action_cb{[&ball = m_pending.ball] {
		ball.starting_count =
			static_cast<std::uint8_t>(std::min(ball.starting_count + engine::keymods_choose(1, 5, 10), static_cast<int>(ball.max_count)));
	}};
	const action_callback max_count_dec_action_cb{[&ball = m_pending.ball] {
		ball.max_count =
			static_cast<std::uint8_t>(std::max(ball.max_count - engine::keymods_choose(1, 5, 10), static_cast<int>(ball.starting_count)));
	}};
	const action_callback max_count_inc_action_cb{[&max_count = m_pending.ball.max_count] {
		max_count = static_cast<std::uint8_t>(std::min(max_count + engine::keymods_choose(1, 5, 10), 255));
	}};
	const action_callback spawn_interval_dec_action_cb{[&spawn_interval = m_pending.ball.spawn_interval] {
		spawn_interval = static_cast<ticks>(
			std::max(static_cast<int>(spawn_interval - engine::keymods_choose(0.1_s, 1_s, 10_s)), static_cast<int>(1_s)));
	}};
	const action_callback spawn_interval_inc_action_cb{[&spawn_interval = m_pending.ball.spawn_interval] {
		spawn_interval = std::min(spawn_interval + engine::keymods_choose(0.1_s, 1_s, 10_s), 60_s);
	}};
	const action_callback initial_size_dec_action_cb{[&initial_size = m_pending.ball.initial_size] {
		initial_size = std::max(initial_size - engine::keymods_choose(1, 10, 100), 10.0f);
	}};
	const action_callback initial_size_inc_action_cb{[&initial_size = m_pending.ball.initial_size] {
		initial_size = std::min(initial_size + engine::keymods_choose(1, 10, 100), 250.0f);
	}};
	const action_callback size_step_dec_action_cb{
		[&size_step = m_pending.ball.size_step] { size_step = std::max(size_step - engine::keymods_choose(0.1f, 1.0f, 10.0f), 0.0f); }};
	const action_callback size_step_inc_action_cb{
		[&size_step = m_pending.ball.size_step] { size_step = std::min(size_step + engine::keymods_choose(0.1f, 1.0f, 10.0f), 50.0f); }};
	const action_callback initial_velocity_dec_action_cb{[&initial_velocity = m_pending.ball.initial_velocity] {
		initial_velocity = std::max(initial_velocity - engine::keymods_choose(1, 10, 100), 100.0f);
	}};
	const action_callback initial_velocity_inc_action_cb{[&initial_velocity = m_pending.ball.initial_velocity] {
		initial_velocity = std::min(initial_velocity + engine::keymods_choose(1, 10, 100), 5000.0f);
	}};
	const action_callback velocity_step_dec_action_cb{[&velocity_step = m_pending.ball.velocity_step] {
		velocity_step = std::max(velocity_step - engine::keymods_choose(1, 10, 100), 0.0f);
	}};
	const action_callback velocity_step_inc_action_cb{[&velocity_step = m_pending.ball.velocity_step] {
		velocity_step = std::min(velocity_step + engine::keymods_choose(1, 10, 100), 1000.0f);
	}};
	const action_callback exit_action_cb{[this] {
		m_substate = substate::EXITING;
		m_timer = 0;
		set_up_exit_animation();
	}};

	// TEXT CALLBACKS

	const text_callback cur_starting_count_text_cb{[this](auto&) { return std::to_string(m_pending.ball.starting_count); }};
	const text_callback cur_max_count_text_cb{[this](auto&) { return std::to_string(m_pending.ball.max_count); }};
	const text_callback cur_spawn_interval_text_cb{[this](auto&) { return std::format("{:.1f}s", m_pending.ball.spawn_interval / 1_sf); }};
	const text_callback cur_initial_size_text_cb{[this](auto&) { return std::format("{:.0f}", m_pending.ball.initial_size); }};
	const text_callback cur_size_step_text_cb{[this](auto&) { return std::format("{:.1f}", m_pending.ball.size_step); }};
	const text_callback cur_initial_velocity_text_cb{[this](auto&) { return std::format("{:.0f}", m_pending.ball.initial_velocity); }};
	const text_callback cur_velocity_step_text_cb{[this](auto&) { return std::format("{:.0f}", m_pending.ball.velocity_step); }};

	//

	widget& title{m_ui.emplace<text_widget>("gamemode_designer", TITLE_POS, tr::align::TOP_CENTER, font::LANGUAGE,
											tr::system::ttf_style::NORMAL, 64)};
	title.unhide();

	widget& subtitle{m_ui.emplace<text_widget>("ball_settings", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE,
											   tr::system::ttf_style::NORMAL, 32)};
	subtitle.pos.change(interp_mode::CUBE, {500, TITLE_POS.y + 64}, 0.5_s);
	subtitle.unhide(0.5_s);

	widget& starting_count_dec{m_ui.emplace<arrow_widget>("starting_count_dec", STARTING_COUNT_START_POS, tr::align::CENTER_LEFT, false,
														  starting_count_dec_status_cb, starting_count_dec_action_cb)};
	widget& starting_count_inc{m_ui.emplace<arrow_widget>("starting_count_inc", STARTING_COUNT_START_POS, tr::align::CENTER_RIGHT, true,
														  starting_count_inc_status_cb, starting_count_inc_action_cb)};
	widget& cur_starting_count{m_ui.emplace<text_widget>("cur_starting_count", STARTING_COUNT_START_POS, tr::align::CENTER, font::LANGUAGE,
														 tr::system::ttf_style::NORMAL, 48, cur_starting_count_text_cb)};
	starting_count_dec.pos.change(interp_mode::CUBE, {765, STARTING_COUNT_START_POS.y}, 0.5_s);
	starting_count_inc.pos.change(interp_mode::CUBE, {985, STARTING_COUNT_START_POS.y}, 0.5_s);
	cur_starting_count.pos.change(interp_mode::CUBE, {875.5, STARTING_COUNT_START_POS.y}, 0.5_s);
	starting_count_dec.unhide(0.5_s);
	starting_count_inc.unhide(0.5_s);
	cur_starting_count.unhide(0.5_s);

	widget& max_count_dec{m_ui.emplace<arrow_widget>("max_count_dec", MAX_COUNT_START_POS, tr::align::CENTER_LEFT, false,
													 max_count_dec_status_cb, max_count_dec_action_cb)};
	widget& max_count_inc{m_ui.emplace<arrow_widget>("max_count_inc", MAX_COUNT_START_POS, tr::align::CENTER_RIGHT, true,
													 max_count_inc_status_cb, max_count_inc_action_cb)};
	widget& cur_max_count{m_ui.emplace<text_widget>("cur_max_count", MAX_COUNT_START_POS, tr::align::CENTER, font::LANGUAGE,
													tr::system::ttf_style::NORMAL, 48, cur_max_count_text_cb)};
	max_count_dec.pos.change(interp_mode::CUBE, {765, MAX_COUNT_START_POS.y}, 0.5_s);
	max_count_inc.pos.change(interp_mode::CUBE, {985, MAX_COUNT_START_POS.y}, 0.5_s);
	cur_max_count.pos.change(interp_mode::CUBE, {875.5, MAX_COUNT_START_POS.y}, 0.5_s);
	max_count_dec.unhide(0.5_s);
	max_count_inc.unhide(0.5_s);
	cur_max_count.unhide(0.5_s);

	widget& spawn_interval_dec{m_ui.emplace<arrow_widget>("spawn_interval_dec", SPAWN_INTERVAL_START_POS, tr::align::CENTER_LEFT, false,
														  spawn_interval_dec_status_cb, spawn_interval_dec_action_cb)};
	widget& spawn_interval_inc{m_ui.emplace<arrow_widget>("spawn_interval_inc", SPAWN_INTERVAL_START_POS, tr::align::CENTER_RIGHT, true,
														  spawn_interval_inc_status_cb, spawn_interval_inc_action_cb)};
	widget& cur_spawn_interval{m_ui.emplace<text_widget>("cur_spawn_interval", SPAWN_INTERVAL_START_POS, tr::align::CENTER, font::LANGUAGE,
														 tr::system::ttf_style::NORMAL, 48, cur_spawn_interval_text_cb)};
	spawn_interval_dec.pos.change(interp_mode::CUBE, {765, SPAWN_INTERVAL_START_POS.y}, 0.5_s);
	spawn_interval_inc.pos.change(interp_mode::CUBE, {985, SPAWN_INTERVAL_START_POS.y}, 0.5_s);
	cur_spawn_interval.pos.change(interp_mode::CUBE, {875.5, SPAWN_INTERVAL_START_POS.y}, 0.5_s);
	spawn_interval_dec.unhide(0.5_s);
	spawn_interval_inc.unhide(0.5_s);
	cur_spawn_interval.unhide(0.5_s);

	widget& initial_size_dec{m_ui.emplace<arrow_widget>("initial_size_dec", INITIAL_SIZE_START_POS, tr::align::CENTER_LEFT, false,
														initial_size_dec_status_cb, initial_size_dec_action_cb)};
	widget& initial_size_inc{m_ui.emplace<arrow_widget>("initial_size_inc", INITIAL_SIZE_START_POS, tr::align::CENTER_RIGHT, true,
														initial_size_inc_status_cb, initial_size_inc_action_cb)};
	widget& cur_initial_size{m_ui.emplace<text_widget>("cur_initial_size", INITIAL_SIZE_START_POS, tr::align::CENTER, font::LANGUAGE,
													   tr::system::ttf_style::NORMAL, 48, cur_initial_size_text_cb)};
	initial_size_dec.pos.change(interp_mode::CUBE, {765, INITIAL_SIZE_START_POS.y}, 0.5_s);
	initial_size_inc.pos.change(interp_mode::CUBE, {985, INITIAL_SIZE_START_POS.y}, 0.5_s);
	cur_initial_size.pos.change(interp_mode::CUBE, {875.5, INITIAL_SIZE_START_POS.y}, 0.5_s);
	initial_size_dec.unhide(0.5_s);
	initial_size_inc.unhide(0.5_s);
	cur_initial_size.unhide(0.5_s);

	widget& size_step_dec{m_ui.emplace<arrow_widget>("size_step_dec", SIZE_STEP_START_POS, tr::align::CENTER_LEFT, false,
													 size_step_dec_status_cb, size_step_dec_action_cb)};
	widget& size_step_inc{m_ui.emplace<arrow_widget>("size_step_inc", SIZE_STEP_START_POS, tr::align::CENTER_RIGHT, true,
													 size_step_inc_status_cb, size_step_inc_action_cb)};
	widget& cur_size_step{m_ui.emplace<text_widget>("cur_size_step", SIZE_STEP_START_POS, tr::align::CENTER, font::LANGUAGE,
													tr::system::ttf_style::NORMAL, 48, cur_size_step_text_cb)};
	size_step_dec.pos.change(interp_mode::CUBE, {765, SIZE_STEP_START_POS.y}, 0.5_s);
	size_step_inc.pos.change(interp_mode::CUBE, {985, SIZE_STEP_START_POS.y}, 0.5_s);
	cur_size_step.pos.change(interp_mode::CUBE, {875.5, SIZE_STEP_START_POS.y}, 0.5_s);
	size_step_dec.unhide(0.5_s);
	size_step_inc.unhide(0.5_s);
	cur_size_step.unhide(0.5_s);

	widget& initial_velocity_dec{m_ui.emplace<arrow_widget>("initial_velocity_dec", INITIAL_VELOCITY_START_POS, tr::align::CENTER_LEFT,
															false, initial_velocity_dec_status_cb, initial_velocity_dec_action_cb)};
	widget& initial_velocity_inc{m_ui.emplace<arrow_widget>("initial_velocity_inc", INITIAL_VELOCITY_START_POS, tr::align::CENTER_RIGHT,
															true, initial_velocity_inc_status_cb, initial_velocity_inc_action_cb)};
	widget& cur_initial_velocity{m_ui.emplace<text_widget>("cur_initial_velocity", INITIAL_VELOCITY_START_POS, tr::align::CENTER,
														   font::LANGUAGE, tr::system::ttf_style::NORMAL, 48,
														   cur_initial_velocity_text_cb)};
	initial_velocity_dec.pos.change(interp_mode::CUBE, {765, INITIAL_VELOCITY_START_POS.y}, 0.5_s);
	initial_velocity_inc.pos.change(interp_mode::CUBE, {985, INITIAL_VELOCITY_START_POS.y}, 0.5_s);
	cur_initial_velocity.pos.change(interp_mode::CUBE, {875.5, INITIAL_VELOCITY_START_POS.y}, 0.5_s);
	initial_velocity_dec.unhide(0.5_s);
	initial_velocity_inc.unhide(0.5_s);
	cur_initial_velocity.unhide(0.5_s);

	widget& velocity_step_dec{m_ui.emplace<arrow_widget>("velocity_step_dec", VELOCITY_STEP_START_POS, tr::align::CENTER_LEFT, false,
														 velocity_step_dec_status_cb, velocity_step_dec_action_cb)};
	widget& velocity_step_inc{m_ui.emplace<arrow_widget>("velocity_step_inc", VELOCITY_STEP_START_POS, tr::align::CENTER_RIGHT, true,
														 velocity_step_inc_status_cb, velocity_step_inc_action_cb)};
	widget& cur_velocity_step{m_ui.emplace<text_widget>("cur_velocity_step", VELOCITY_STEP_START_POS, tr::align::CENTER, font::LANGUAGE,
														tr::system::ttf_style::NORMAL, 48, cur_velocity_step_text_cb)};
	velocity_step_dec.pos.change(interp_mode::CUBE, {765, VELOCITY_STEP_START_POS.y}, 0.5_s);
	velocity_step_inc.pos.change(interp_mode::CUBE, {985, VELOCITY_STEP_START_POS.y}, 0.5_s);
	cur_velocity_step.pos.change(interp_mode::CUBE, {875.5, VELOCITY_STEP_START_POS.y}, 0.5_s);
	velocity_step_dec.unhide(0.5_s);
	velocity_step_inc.unhide(0.5_s);
	cur_velocity_step.unhide(0.5_s);

	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const label& label{LABELS[i]};
		const glm::vec2 pos{-50, 298 + i * 75};
		widget& widget{m_ui.emplace<text_widget>(label.tag, pos, tr::align::CENTER_LEFT, LABELS[i].tooltip, font::LANGUAGE,
												 tr::system::ttf_style::NORMAL, 48)};
		widget.pos.change(interp_mode::CUBE, {15, 298 + i * 75}, 0.5_s);
		widget.unhide(0.5_s);
	}

	widget& exit{m_ui.emplace<clickable_text_widget>("exit", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													 DEFAULT_TEXT_CALLBACK, status_cb, exit_action_cb, NO_TOOLTIP, EXIT_SHORTCUTS,
													 sound::CANCEL)};
	exit.pos.change(interp_mode::CUBE, {500, 1000}, 0.5_s);
	exit.unhide(0.5_s);
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
	widget& subtitle{m_ui.get("ball_settings")};
	widget& exit{m_ui.get("exit")};
	subtitle.pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	subtitle.hide(0.5_s);
	for (const char* tag : tr::project(LABELS, &label::tag)) {
		widget& widget{m_ui.get(tag)};
		widget.pos.change(interp_mode::CUBE, {-50, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	for (const char* tag : RIGHT_WIDGETS) {
		widget& widget{m_ui.get(tag)};
		widget.pos.change(interp_mode::CUBE, {1050, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	exit.pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	exit.hide(0.5_s);
}