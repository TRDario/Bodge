#include "../../include/state/ball_settings_editor_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"

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
constexpr std::initializer_list<key_chord> EXIT_SHORTCUTS{
	{tr::keycode::C}, {tr::keycode::Q}, {tr::keycode::E}, {tr::keycode::ESCAPE}, {tr::keycode::TOP_ROW_2},
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
	: _substate{substate::IN_EDITOR}, _timer{0}, _game{std::move(game)}, _gamemode{gamemode}
{
	// STATUS CALLBACKS

	const status_callback status_cb{[this] { return _substate == substate::IN_EDITOR; }};
	const status_callback starting_count_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.starting_count > 0; }};
	const status_callback starting_count_inc_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.starting_count < _gamemode.ball.max_count; }};
	const status_callback max_count_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.max_count > _gamemode.ball.starting_count; }};
	const status_callback max_count_inc_status_cb{[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.max_count < 255; }};
	const status_callback spawn_interval_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.spawn_interval > 1.0_s; }};
	const status_callback spawn_interval_inc_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.spawn_interval < 60_s; }};
	const status_callback initial_size_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.initial_size > 10; }};
	const status_callback initial_size_inc_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.initial_size < 250.0f; }};
	const status_callback size_step_dec_status_cb{[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.size_step > 0; }};
	const status_callback size_step_inc_status_cb{[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.size_step < 50.0f; }};
	const status_callback initial_velocity_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.initial_velocity > 100; }};
	const status_callback initial_velocity_inc_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.initial_velocity < 5000.0f; }};
	const status_callback velocity_step_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.velocity_step > 0; }};
	const status_callback velocity_step_inc_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.ball.velocity_step < 1000.0f; }};

	// ACTION CALLBACKS

	const action_callback starting_count_dec_action_cb{[&starting_count = _gamemode.ball.starting_count] {
		const int delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10 : 1};
		starting_count = static_cast<std::uint8_t>(std::max(starting_count - delta, 0));
	}};
	const action_callback starting_count_inc_action_cb{[&ball = _gamemode.ball] {
		const int delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10 : 1};
		ball.starting_count = static_cast<std::uint8_t>(std::min(ball.starting_count + delta, static_cast<int>(ball.max_count)));
	}};
	const action_callback max_count_dec_action_cb{[&ball = _gamemode.ball] {
		const int delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10 : 1};
		ball.max_count = static_cast<std::uint8_t>(std::max(ball.max_count - delta, static_cast<int>(ball.starting_count)));
	}};
	const action_callback max_count_inc_action_cb{[&max_count = _gamemode.ball.max_count] {
		const int delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10 : 1};
		max_count = static_cast<std::uint8_t>(std::min(max_count + delta, 255));
	}};
	const action_callback spawn_interval_dec_action_cb{[&spawn_interval = _gamemode.ball.spawn_interval] {
		const ticks delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10_s : 1_s};
		spawn_interval = static_cast<ticks>(std::max(static_cast<int>(spawn_interval - delta), static_cast<int>(1_s)));
	}};
	const action_callback spawn_interval_inc_action_cb{[&spawn_interval = _gamemode.ball.spawn_interval] {
		const ticks delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10_s : 1_s};
		spawn_interval = std::min(spawn_interval + delta, 60_s);
	}};
	const action_callback initial_size_dec_action_cb{[&initial_size = _gamemode.ball.initial_size] {
		const float delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10.0f : 1.0f};
		initial_size = std::max(initial_size - delta, 10.0f);
	}};
	const action_callback initial_size_inc_action_cb{[&initial_size = _gamemode.ball.initial_size] {
		const float delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10.0f : 1.0f};
		initial_size = std::min(initial_size + delta, 250.0f);
	}};
	const action_callback size_step_dec_action_cb{[&size_step = _gamemode.ball.size_step] {
		const float delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10.0f : 1.0f};
		size_step = std::max(size_step - delta, 0.0f);
	}};
	const action_callback size_step_inc_action_cb{[&size_step = _gamemode.ball.size_step] {
		const float delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 10.0f : 1.0f};
		size_step = std::min(size_step + delta, 50.0f);
	}};
	const action_callback initial_velocity_dec_action_cb{[&initial_velocity = _gamemode.ball.initial_velocity] {
		const float delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 100.0f : 10.0f};
		initial_velocity = std::max(initial_velocity - delta, 100.0f);
	}};
	const action_callback initial_velocity_inc_action_cb{[&initial_velocity = _gamemode.ball.initial_velocity] {
		const float delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 100.0f : 10.0f};
		initial_velocity = std::min(initial_velocity + delta, 5000.0f);
	}};
	const action_callback velocity_step_dec_action_cb{[&velocity_step = _gamemode.ball.velocity_step] {
		const float delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 100.0f : 10.0f};
		velocity_step = std::max(velocity_step - delta, 0.0f);
	}};
	const action_callback velocity_step_inc_action_cb{[&velocity_step = _gamemode.ball.velocity_step] {
		const float delta{tr::keyboard::held_mods() & tr::keymods::SHIFT ? 100.0f : 10.0f};
		velocity_step = std::min(velocity_step + delta, 1000.0f);
	}};
	const action_callback exit_action_cb{[this] {
		_substate = substate::EXITING;
		_timer = 0;
		set_up_exit_animation();
	}};

	// TEXT CALLBACKS

	const text_callback cur_starting_count_text_cb{[this](auto&) { return std::to_string(_gamemode.ball.starting_count); }};
	const text_callback cur_max_count_text_cb{[this](auto&) { return std::to_string(_gamemode.ball.max_count); }};
	const text_callback cur_spawn_interval_text_cb{[this](auto&) { return std::format("{}s", _gamemode.ball.spawn_interval / 1_s); }};
	const text_callback cur_initial_size_text_cb{[this](auto&) { return std::format("{:.0f}", _gamemode.ball.initial_size); }};
	const text_callback cur_size_step_text_cb{[this](auto&) { return std::format("{:.0f}", _gamemode.ball.size_step); }};
	const text_callback cur_initial_velocity_text_cb{[this](auto&) { return std::format("{:.0f}", _gamemode.ball.initial_velocity); }};
	const text_callback cur_velocity_step_text_cb{[this](auto&) { return std::format("{:.0f}", _gamemode.ball.velocity_step); }};

	//

	widget& title{
		_ui.emplace<text_widget>("gamemode_designer", TITLE_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.unhide();

	widget& subtitle{
		_ui.emplace<text_widget>("ball_settings", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 32)};
	subtitle.pos.change({500, TITLE_POS.y + 64}, 0.5_s);
	subtitle.unhide(0.5_s);

	widget& starting_count_dec{_ui.emplace<arrow_widget>("starting_count_dec", STARTING_COUNT_START_POS, tr::align::CENTER_LEFT, false,
														 starting_count_dec_status_cb, starting_count_dec_action_cb)};
	widget& starting_count_inc{_ui.emplace<arrow_widget>("starting_count_inc", STARTING_COUNT_START_POS, tr::align::CENTER_RIGHT, true,
														 starting_count_inc_status_cb, starting_count_inc_action_cb)};
	widget& cur_starting_count{_ui.emplace<text_widget>("cur_starting_count", STARTING_COUNT_START_POS, tr::align::CENTER, font::LANGUAGE,
														tr::ttf_style::NORMAL, 48, cur_starting_count_text_cb)};
	starting_count_dec.pos.change({800, STARTING_COUNT_START_POS.y}, 0.5_s);
	starting_count_inc.pos.change({985, STARTING_COUNT_START_POS.y}, 0.5_s);
	cur_starting_count.pos.change({892.5, STARTING_COUNT_START_POS.y}, 0.5_s);
	starting_count_dec.unhide(0.5_s);
	starting_count_inc.unhide(0.5_s);
	cur_starting_count.unhide(0.5_s);

	widget& max_count_dec{_ui.emplace<arrow_widget>("max_count_dec", MAX_COUNT_START_POS, tr::align::CENTER_LEFT, false,
													max_count_dec_status_cb, max_count_dec_action_cb)};
	widget& max_count_inc{_ui.emplace<arrow_widget>("max_count_inc", MAX_COUNT_START_POS, tr::align::CENTER_RIGHT, true,
													max_count_inc_status_cb, max_count_inc_action_cb)};
	widget& cur_max_count{_ui.emplace<text_widget>("cur_max_count", MAX_COUNT_START_POS, tr::align::CENTER, font::LANGUAGE,
												   tr::ttf_style::NORMAL, 48, cur_max_count_text_cb)};
	max_count_dec.pos.change({800, MAX_COUNT_START_POS.y}, 0.5_s);
	max_count_inc.pos.change({985, MAX_COUNT_START_POS.y}, 0.5_s);
	cur_max_count.pos.change({892.5, MAX_COUNT_START_POS.y}, 0.5_s);
	max_count_dec.unhide(0.5_s);
	max_count_inc.unhide(0.5_s);
	cur_max_count.unhide(0.5_s);

	widget& spawn_interval_dec{_ui.emplace<arrow_widget>("spawn_interval_dec", SPAWN_INTERVAL_START_POS, tr::align::CENTER_LEFT, false,
														 spawn_interval_dec_status_cb, spawn_interval_dec_action_cb)};
	widget& spawn_interval_inc{_ui.emplace<arrow_widget>("spawn_interval_inc", SPAWN_INTERVAL_START_POS, tr::align::CENTER_RIGHT, true,
														 spawn_interval_inc_status_cb, spawn_interval_inc_action_cb)};
	widget& cur_spawn_interval{_ui.emplace<text_widget>("cur_spawn_interval", SPAWN_INTERVAL_START_POS, tr::align::CENTER, font::LANGUAGE,
														tr::ttf_style::NORMAL, 48, cur_spawn_interval_text_cb)};
	spawn_interval_dec.pos.change({800, SPAWN_INTERVAL_START_POS.y}, 0.5_s);
	spawn_interval_inc.pos.change({985, SPAWN_INTERVAL_START_POS.y}, 0.5_s);
	cur_spawn_interval.pos.change({892.5, SPAWN_INTERVAL_START_POS.y}, 0.5_s);
	spawn_interval_dec.unhide(0.5_s);
	spawn_interval_inc.unhide(0.5_s);
	cur_spawn_interval.unhide(0.5_s);

	widget& initial_size_dec{_ui.emplace<arrow_widget>("initial_size_dec", INITIAL_SIZE_START_POS, tr::align::CENTER_LEFT, false,
													   initial_size_dec_status_cb, initial_size_dec_action_cb)};
	widget& initial_size_inc{_ui.emplace<arrow_widget>("initial_size_inc", INITIAL_SIZE_START_POS, tr::align::CENTER_RIGHT, true,
													   initial_size_inc_status_cb, initial_size_inc_action_cb)};
	widget& cur_initial_size{_ui.emplace<text_widget>("cur_initial_size", INITIAL_SIZE_START_POS, tr::align::CENTER, font::LANGUAGE,
													  tr::ttf_style::NORMAL, 48, cur_initial_size_text_cb)};
	initial_size_dec.pos.change({800, INITIAL_SIZE_START_POS.y}, 0.5_s);
	initial_size_inc.pos.change({985, INITIAL_SIZE_START_POS.y}, 0.5_s);
	cur_initial_size.pos.change({892.5, INITIAL_SIZE_START_POS.y}, 0.5_s);
	initial_size_dec.unhide(0.5_s);
	initial_size_inc.unhide(0.5_s);
	cur_initial_size.unhide(0.5_s);

	widget& size_step_dec{_ui.emplace<arrow_widget>("size_step_dec", SIZE_STEP_START_POS, tr::align::CENTER_LEFT, false,
													size_step_dec_status_cb, size_step_dec_action_cb)};
	widget& size_step_inc{_ui.emplace<arrow_widget>("size_step_inc", SIZE_STEP_START_POS, tr::align::CENTER_RIGHT, true,
													size_step_inc_status_cb, size_step_inc_action_cb)};
	widget& cur_size_step{_ui.emplace<text_widget>("cur_size_step", SIZE_STEP_START_POS, tr::align::CENTER, font::LANGUAGE,
												   tr::ttf_style::NORMAL, 48, cur_size_step_text_cb)};
	size_step_dec.pos.change({830, SIZE_STEP_START_POS.y}, 0.5_s);
	size_step_inc.pos.change({985, SIZE_STEP_START_POS.y}, 0.5_s);
	cur_size_step.pos.change({907.5, SIZE_STEP_START_POS.y}, 0.5_s);
	size_step_dec.unhide(0.5_s);
	size_step_inc.unhide(0.5_s);
	cur_size_step.unhide(0.5_s);

	widget& initial_velocity_dec{_ui.emplace<arrow_widget>("initial_velocity_dec", INITIAL_VELOCITY_START_POS, tr::align::CENTER_LEFT,
														   false, initial_velocity_dec_status_cb, initial_velocity_dec_action_cb)};
	widget& initial_velocity_inc{_ui.emplace<arrow_widget>("initial_velocity_inc", INITIAL_VELOCITY_START_POS, tr::align::CENTER_RIGHT,
														   true, initial_velocity_inc_status_cb, initial_velocity_inc_action_cb)};
	widget& cur_initial_velocity{_ui.emplace<text_widget>("cur_initial_velocity", INITIAL_VELOCITY_START_POS, tr::align::CENTER,
														  font::LANGUAGE, tr::ttf_style::NORMAL, 48, cur_initial_velocity_text_cb)};
	initial_velocity_dec.pos.change({765, INITIAL_VELOCITY_START_POS.y}, 0.5_s);
	initial_velocity_inc.pos.change({985, INITIAL_VELOCITY_START_POS.y}, 0.5_s);
	cur_initial_velocity.pos.change({875.5, INITIAL_VELOCITY_START_POS.y}, 0.5_s);
	initial_velocity_dec.unhide(0.5_s);
	initial_velocity_inc.unhide(0.5_s);
	cur_initial_velocity.unhide(0.5_s);

	widget& velocity_step_dec{_ui.emplace<arrow_widget>("velocity_step_dec", VELOCITY_STEP_START_POS, tr::align::CENTER_LEFT, false,
														velocity_step_dec_status_cb, velocity_step_dec_action_cb)};
	widget& velocity_step_inc{_ui.emplace<arrow_widget>("velocity_step_inc", VELOCITY_STEP_START_POS, tr::align::CENTER_RIGHT, true,
														velocity_step_inc_status_cb, velocity_step_inc_action_cb)};
	widget& cur_velocity_step{_ui.emplace<text_widget>("cur_velocity_step", VELOCITY_STEP_START_POS, tr::align::CENTER, font::LANGUAGE,
													   tr::ttf_style::NORMAL, 48, cur_velocity_step_text_cb)};
	velocity_step_dec.pos.change({765, VELOCITY_STEP_START_POS.y}, 0.5_s);
	velocity_step_inc.pos.change({985, VELOCITY_STEP_START_POS.y}, 0.5_s);
	cur_velocity_step.pos.change({875.5, VELOCITY_STEP_START_POS.y}, 0.5_s);
	velocity_step_dec.unhide(0.5_s);
	velocity_step_inc.unhide(0.5_s);
	cur_velocity_step.unhide(0.5_s);

	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const label& label{LABELS[i]};
		const glm::vec2 pos{-50, 298 + i * 75};
		widget& widget{
			_ui.emplace<text_widget>(label.tag, pos, tr::align::CENTER_LEFT, LABELS[i].tooltip, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
		widget.pos.change({15, 298 + i * 75}, 0.5_s);
		widget.unhide(0.5_s);
	}

	widget& exit{_ui.emplace<clickable_text_widget>("exit", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													DEFAULT_TEXT_CALLBACK, status_cb, exit_action_cb, NO_TOOLTIP, EXIT_SHORTCUTS,
													sfx::CANCEL)};
	exit.pos.change({500, 1000}, 0.5_s);
	exit.unhide(0.5_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> ball_settings_editor_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> ball_settings_editor_state::update(tr::duration)
{
	++_timer;
	_game->update({});
	_ui.update();

	switch (_substate) {
	case substate::IN_EDITOR:
		return nullptr;
	case substate::EXITING:
		return _timer >= 0.5_s ? std::make_unique<gamemode_designer_state>(std::move(_game), _gamemode, true) : nullptr;
	}
}

void ball_settings_editor_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void ball_settings_editor_state::set_up_exit_animation() noexcept
{
	widget& subtitle{_ui.get("ball_settings")};
	widget& exit{_ui.get("exit")};
	subtitle.pos.change(TOP_START_POS, 0.5_s);
	subtitle.hide(0.5_s);
	for (const char* tag : tr::project(LABELS, &label::tag)) {
		widget& widget{_ui.get(tag)};
		widget.pos.change({-50, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	for (const char* tag : RIGHT_WIDGETS) {
		widget& widget{_ui.get(tag)};
		widget.pos.change({1050, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	exit.pos.change(BOTTOM_START_POS, 0.5_s);
	exit.hide(0.5_s);
}