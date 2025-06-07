#include "../../include/engine.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/pause_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr array<const char*, 4> BUTTONS{"unpause", "restart", "save_and_quit", "quit"};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

pause_state::pause_state(unique_ptr<active_game>&& game, bool blur_in) noexcept
	: _substate{blur_in ? substate::PAUSING : substate::PAUSED}, _timer{0}, _game{std::move(game)}
{
	if (blur_in) {
		_game->add_to_renderer();
		tr::renderer_2d::draw(engine::blur_renderer().input());
	}

	widget& title{_ui.emplace<text_widget>("paused", vec2{500, -50}, CENTER, font::LANGUAGE, ttf_style::NORMAL, 64)};
	title.pos.change({500, 500 - (BUTTONS.size() + 1) * 30}, 0.5_s);
	title.unhide(0.5_s);

	const status_callback status_cb{[this] { return _substate == substate::PAUSED || _substate == substate::PAUSING; }};
	array<action_callback, BUTTONS.size()> action_cbs{
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
			_substate = substate::SAVING_AND_QUITTING;
			set_up_exit_animation();
		},
		[this] {
			_timer = 0;
			_substate = substate::QUITTING;
			set_up_exit_animation();
		},
	};
	array<vector<key_chord>, BUTTONS.size()> chords{{
		{{key::ESCAPE}, {key::TOP_ROW_1}},
		{{key::R}, {key::TOP_ROW_2}},
		{{key::S}, {key::TOP_ROW_3}},
		{{key::Q}, {key::E}, {key::TOP_ROW_4}},
	}};
	for (size_t i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * rand(rng, 250.0f, 400.0f)};
		const vec2 pos{500 + offset, 500 - (BUTTONS.size() + 1) * 30 + (i + 2) * 60};
		widget& widget{_ui.emplace<clickable_text_widget>(BUTTONS[i], pos, CENTER, font::LANGUAGE, 48, DEFAULT_TEXT_CALLBACK, status_cb,
														  std::move(action_cbs[i]), NO_TOOLTIP, std::move(chords[i]))};
		widget.pos.change({500, pos.y}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

u32 pause_state::type() const noexcept
{
	return ID;
}

unique_ptr<state> pause_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

unique_ptr<state> pause_state::update(tr::duration)
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
		return _timer >= 0.5_s ? make_unique<game_state>(std::move(_game), false) : nullptr;
	case substate::RESTARTING:
		return _timer >= 0.5_s ? make_unique<game_state>(make_unique<active_game>(_game->gamemode()), true) : nullptr;
	case substate::SAVING_AND_QUITTING:
		return _timer >= 0.5_s ? make_unique<title_state>() : nullptr;
	case substate::QUITTING:
		return _timer >= 0.5_s ? make_unique<title_state>() : nullptr;
	}
}

void pause_state::draw()
{
	engine::blur_renderer().draw(saturation_factor(), blur_strength());
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(_substate == substate::RESTARTING || _substate == substate::QUITTING ? _timer / 0.5_sf : 0);
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float pause_state::saturation_factor() const noexcept
{
	switch (_substate) {
	case substate::PAUSED:
	case substate::RESTARTING:
	case substate::SAVING_AND_QUITTING:
	case substate::QUITTING:
		return 0.35f;
	case substate::PAUSING:
		return 1 - _timer / 0.5_sf * 0.65f;
	case substate::UNPAUSING:
		return 0.35f + _timer / 0.5_sf * 0.65f;
	}
}

float pause_state::blur_strength() const noexcept
{
	switch (_substate) {
	case substate::PAUSED:
	case substate::RESTARTING:
	case substate::SAVING_AND_QUITTING:
	case substate::QUITTING:
		return 10;
	case substate::PAUSING:
		return _timer / 0.5_sf * 10;
	case substate::UNPAUSING:
		return (1 - _timer / 0.5_sf) * 10;
	}
}

void pause_state::set_up_exit_animation() noexcept
{
	_ui.get("paused").pos.change({500, -50}, 0.5_s);
	for (size_t i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * rand(rng, 250.0f, 400.0f)};
		widget& widget{_ui.get(BUTTONS[i])};
		widget.pos.change(vec2{widget.pos} + vec2{offset, 0}, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}