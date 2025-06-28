#include "../../include/engine.hpp"
#include "../../include/state/replay_pause_state.hpp"
#include "../../include/state/replay_state.hpp"
#include "../../include/state/replays_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The pause screen buttons.
constexpr std::array<const char*, 3> BUTTONS{"unpause", "restart", "quit"};
// Shortcuts of the buttons.
constexpr std::array<std::initializer_list<key_chord>, BUTTONS.size()> SHORTCUTS{{
	{{tr::keycode::ESCAPE}, {tr::keycode::TOP_ROW_1}},
	{{tr::keycode::R}, {tr::keycode::TOP_ROW_2}},
	{{tr::keycode::Q}, {tr::keycode::E}, {tr::keycode::TOP_ROW_3}},
}};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

replay_pause_state::replay_pause_state(std::unique_ptr<replay_game>&& game, bool blur_in) noexcept
	: _substate{blur_in ? substate::PAUSING : substate::PAUSED}, _timer{0}, _game{std::move(game)}
{
	if (blur_in) {
		_game->add_to_renderer();
		tr::renderer_2d::draw(engine::blur_renderer().input());
	}

	constexpr float TITLE_Y{500 - (BUTTONS.size() + 1) * 30};
	widget& title{_ui.emplace<text_widget>("replay_paused", glm::vec2{500, TITLE_Y - 100}, tr::align::CENTER, font::LANGUAGE,
										   tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, TITLE_Y}, 0.5_s);
	title.unhide(0.5_s);

	const status_callback status_cb{[this] { return _substate == substate::PAUSED || _substate == substate::PAUSING; }};
	const status_callback unpause_status_cb{[this] { return _substate == substate::PAUSED; }};
	std::array<action_callback, BUTTONS.size()> action_cbs{
		[this] {
			_timer = 0;
			_substate = substate::UNPAUSING;
			set_up_exit_animation();
		},
		[this] {
			_timer = 0;
			_substate = substate::RESTARTING;
			set_up_exit_animation();
		},
		[this] {
			_timer = 0;
			_substate = substate::QUITTING;
			set_up_exit_animation();
		},
	};
	for (std::size_t i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * tr::rand(rng, 50.0f, 150.0f)};
		const glm::vec2 pos{500 + offset, 500 - (BUTTONS.size() + 1) * 30 + (i + 2) * 60};
		widget& widget{_ui.emplace<clickable_text_widget>(BUTTONS[i], pos, tr::align::CENTER, font::LANGUAGE, 48, DEFAULT_TEXT_CALLBACK,
														  i == 0 ? unpause_status_cb : status_cb, std::move(action_cbs[i]), NO_TOOLTIP,
														  SHORTCUTS[i])};
		widget.pos.change({500, pos.y}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> replay_pause_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> replay_pause_state::update(tr::duration)
{
	++_timer;
	_ui.update();

	switch (_substate) {
	case substate::PAUSING:
		if (_timer >= 0.5_s) {
			_timer = 0;
			_substate = substate::PAUSED;
		}
		return nullptr;
	case substate::PAUSED:
		return nullptr;
	case substate::UNPAUSING:
		return _timer >= 0.5_s ? std::make_unique<replay_state>(std::move(_game), false) : nullptr;
	case substate::RESTARTING:
		return _timer >= 0.5_s ? std::make_unique<replay_state>(std::make_unique<replay_game>(*_game), true) : nullptr;
	case substate::QUITTING:
		return _timer >= 0.5_s ? std::make_unique<replays_state>() : nullptr;
	}
}

void replay_pause_state::draw()
{
	engine::blur_renderer().draw(saturation_factor(), blur_strength());
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(_substate == substate::RESTARTING || _substate == substate::QUITTING ? _timer / 0.5_sf : 0);
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float replay_pause_state::saturation_factor() const noexcept
{
	switch (_substate) {
	case substate::PAUSED:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 0.35f;
	case substate::PAUSING:
		return 1 - _timer / 0.5_sf * 0.65f;
	case substate::UNPAUSING:
		return 0.35f + _timer / 0.5_sf * 0.65f;
	}
}

float replay_pause_state::blur_strength() const noexcept
{
	switch (_substate) {
	case substate::PAUSED:
	case substate::RESTARTING:
	case substate::QUITTING:
		return 10;
	case substate::PAUSING:
		return _timer / 0.5_sf * 10;
	case substate::UNPAUSING:
		return (1 - _timer / 0.5_sf) * 10;
	}
}

void replay_pause_state::set_up_exit_animation() noexcept
{
	_ui.get("replay_paused").pos.change({500, 400 - (BUTTONS.size() + 1) * 30}, 0.5_s);
	for (std::size_t i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * tr::rand(rng, 50.0f, 150.0f)};
		widget& widget{_ui.get(BUTTONS[i])};
		widget.pos.change(glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}