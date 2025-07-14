#include "../../include/state/player_settings_editor_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Left-aligned labels.
constexpr std::array<label, 3> LABELS{{
	{"starting_lives", "starting_lives_tt"},
	{"hitbox_radius", "hitbox_size_tt"},
	{"inertia_factor", "inertia_factor_tt"},
}};
// Right-aligned widgets.
constexpr std::array<const char*, 9> RIGHT_WIDGETS{"starting_lives_dec", "cur_starting_lives", "starting_lives_inc",
												   "hitbox_radius_dec",  "cur_hitbox_radius",  "hitbox_radius_inc",
												   "inertia_factor_dec", "cur_inertia_factor", "inertia_factor_inc"};
// Shortcuts of the exit button.
constexpr std::initializer_list<tr::key_chord> EXIT_SHORTCUTS{
	{tr::keycode::C}, {tr::keycode::Q}, {tr::keycode::E}, {tr::keycode::ESCAPE}, {tr::keycode::TOP_ROW_2},
};

// Starting position of the starting lives widgets.
constexpr glm::vec2 STARTING_LIVES_START_POS{1050, 450};
// Starting position of the hitbox radius widgets.
constexpr glm::vec2 HITBOX_RADIUS_START_POS{1050, 525};
// Starting position of the inertia factor widgets.
constexpr glm::vec2 INERTIA_FACTOR_START_POS{1050, 600};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

player_settings_editor_state::player_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode)
	: _substate{substate::IN_EDITOR}, _timer{0}, _game{std::move(game)}, _gamemode{gamemode}
{
	// STATUS CALLBACKS

	const status_callback status_cb{[this] { return _substate == substate::IN_EDITOR; }};
	const status_callback starting_lives_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.player.starting_lives > 0; }};
	const status_callback starting_lives_inc_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.player.starting_lives < 255; }};
	const status_callback hitbox_radius_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.player.hitbox_radius > 0.0f; }};
	const status_callback hitbox_radius_inc_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.player.hitbox_radius < 100.0f; }};
	const status_callback inertia_factor_dec_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.player.inertia_factor > 0.0f; }};
	const status_callback inertia_factor_inc_status_cb{
		[this] { return _substate == substate::IN_EDITOR && _gamemode.player.inertia_factor < 0.99f; }};

	// ACTION CALLBACKS

	const action_callback starting_lives_dec_action_cb{[&starting_lives = _gamemode.player.starting_lives] {
		starting_lives = static_cast<std::uint32_t>(std::max(static_cast<int>(starting_lives - engine::keymods_choose(1, 5, 10)), 0));
	}};
	const action_callback starting_lives_inc_action_cb{[&starting_lives = _gamemode.player.starting_lives] {
		starting_lives = std::min(starting_lives + engine::keymods_choose(1, 5, 10), std::uint32_t{255});
	}};
	const action_callback hitbox_radius_dec_action_cb{[&hitbox_radius = _gamemode.player.hitbox_radius] {
		hitbox_radius = std::max(hitbox_radius - engine::keymods_choose(1, 5, 10), 0.0f);
	}};
	const action_callback hitbox_radius_inc_action_cb{[&hitbox_radius = _gamemode.player.hitbox_radius] {
		hitbox_radius = std::min(hitbox_radius + engine::keymods_choose(1, 5, 10), 100.0f);
	}};
	const action_callback inertia_factor_dec_action_cb{[&inertia_factor = _gamemode.player.inertia_factor] {
		inertia_factor = std::max(inertia_factor - engine::keymods_choose(0.01f, 0.05f, 0.1f), 0.0f);
	}};
	const action_callback inertia_factor_inc_action_cb{[&inertia_factor = _gamemode.player.inertia_factor] {
		inertia_factor = std::min(inertia_factor + engine::keymods_choose(0.01f, 0.05f, 0.1f), 0.99f);
	}};
	const action_callback exit_action_cb{[this] {
		_substate = substate::EXITING;
		_timer = 0;
		set_up_exit_animation();
	}};

	// TEXT CALLBACKS

	const text_callback cur_starting_lives_text_cb{[this](auto&) { return std::format("{}", _gamemode.player.starting_lives); }};
	const text_callback cur_hitbox_radius_text_cb{[this](auto&) { return std::format("{:.0f}", _gamemode.player.hitbox_radius); }};
	const text_callback cur_inertia_factor_text_cb{[this](auto&) { return std::format("{:.2f}", _gamemode.player.inertia_factor); }};

	//

	widget& title{
		_ui.emplace<text_widget>("gamemode_designer", TITLE_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.unhide();

	widget& subtitle{
		_ui.emplace<text_widget>("player_settings", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 32)};
	subtitle.pos.change({500, TITLE_POS.y + 64}, 0.5_s);
	subtitle.unhide(0.5_s);

	widget& starting_lives_dec{_ui.emplace<arrow_widget>("starting_lives_dec", STARTING_LIVES_START_POS, tr::align::CENTER_LEFT, false,
														 starting_lives_dec_status_cb, starting_lives_dec_action_cb)};
	widget& starting_lives_inc{_ui.emplace<arrow_widget>("starting_lives_inc", STARTING_LIVES_START_POS, tr::align::CENTER_RIGHT, true,
														 starting_lives_inc_status_cb, starting_lives_inc_action_cb)};
	widget& cur_starting_lives{_ui.emplace<text_widget>("cur_starting_lives", STARTING_LIVES_START_POS, tr::align::CENTER, font::LANGUAGE,
														tr::ttf_style::NORMAL, 48, cur_starting_lives_text_cb)};
	starting_lives_dec.pos.change({790, STARTING_LIVES_START_POS.y}, 0.5_s);
	starting_lives_inc.pos.change({985, STARTING_LIVES_START_POS.y}, 0.5_s);
	cur_starting_lives.pos.change({887.5, STARTING_LIVES_START_POS.y}, 0.5_s);
	starting_lives_dec.unhide(0.5_s);
	starting_lives_inc.unhide(0.5_s);
	cur_starting_lives.unhide(0.5_s);

	widget& hitbox_radius_dec{_ui.emplace<arrow_widget>("hitbox_radius_dec", HITBOX_RADIUS_START_POS, tr::align::CENTER_LEFT, false,
														hitbox_radius_dec_status_cb, hitbox_radius_dec_action_cb)};
	widget& hitbox_radius_inc{_ui.emplace<arrow_widget>("hitbox_radius_inc", HITBOX_RADIUS_START_POS, tr::align::CENTER_RIGHT, true,
														hitbox_radius_inc_status_cb, hitbox_radius_inc_action_cb)};
	widget& cur_hitbox_radius{_ui.emplace<text_widget>("cur_hitbox_radius", HITBOX_RADIUS_START_POS, tr::align::CENTER, font::LANGUAGE,
													   tr::ttf_style::NORMAL, 48, cur_hitbox_radius_text_cb)};
	hitbox_radius_dec.pos.change({790, HITBOX_RADIUS_START_POS.y}, 0.5_s);
	hitbox_radius_inc.pos.change({985, HITBOX_RADIUS_START_POS.y}, 0.5_s);
	cur_hitbox_radius.pos.change({887.5, HITBOX_RADIUS_START_POS.y}, 0.5_s);
	hitbox_radius_dec.unhide(0.5_s);
	hitbox_radius_inc.unhide(0.5_s);
	cur_hitbox_radius.unhide(0.5_s);

	widget& inertia_factor_dec{_ui.emplace<arrow_widget>("inertia_factor_dec", INERTIA_FACTOR_START_POS, tr::align::CENTER_LEFT, false,
														 inertia_factor_dec_status_cb, inertia_factor_dec_action_cb)};
	widget& inertia_factor_inc{_ui.emplace<arrow_widget>("inertia_factor_inc", INERTIA_FACTOR_START_POS, tr::align::CENTER_RIGHT, true,
														 inertia_factor_inc_status_cb, inertia_factor_inc_action_cb)};
	widget& cur_inertia_factor{_ui.emplace<text_widget>("cur_inertia_factor", INERTIA_FACTOR_START_POS, tr::align::CENTER, font::LANGUAGE,
														tr::ttf_style::NORMAL, 48, cur_inertia_factor_text_cb)};
	inertia_factor_dec.pos.change({790, INERTIA_FACTOR_START_POS.y}, 0.5_s);
	inertia_factor_inc.pos.change({985, INERTIA_FACTOR_START_POS.y}, 0.5_s);
	cur_inertia_factor.pos.change({887.5, INERTIA_FACTOR_START_POS.y}, 0.5_s);
	inertia_factor_dec.unhide(0.5_s);
	inertia_factor_inc.unhide(0.5_s);
	cur_inertia_factor.unhide(0.5_s);

	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const label& label{LABELS[i]};
		const glm::vec2 pos{-50, 450 + i * 75};
		widget& widget{
			_ui.emplace<text_widget>(label.tag, pos, tr::align::CENTER_LEFT, LABELS[i].tooltip, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
		widget.pos.change({15, 450 + i * 75}, 0.5_s);
		widget.unhide(0.5_s);
	}

	widget& exit{_ui.emplace<clickable_text_widget>("exit", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													DEFAULT_TEXT_CALLBACK, status_cb, exit_action_cb, NO_TOOLTIP, EXIT_SHORTCUTS,
													sfx::CANCEL)};
	exit.pos.change({500, 1000}, 0.5_s);
	exit.unhide(0.5_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> player_settings_editor_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> player_settings_editor_state::update(tr::duration)
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

void player_settings_editor_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void player_settings_editor_state::set_up_exit_animation() noexcept
{
	widget& subtitle{_ui.get("player_settings")};
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