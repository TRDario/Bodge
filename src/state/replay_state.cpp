#include "../../include/engine.hpp"
#include "../../include/state/replay_pause_state.hpp"
#include "../../include/state/replay_state.hpp"
#include "../../include/state/replays_state.hpp"

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

replay_state::replay_state(std::unique_ptr<replay_game>&& game, bool fade_in) noexcept
	: _substate{fade_in ? substate::STARTING : substate::WATCHING}, _timer{0}, _game{std::move(game)}
{
	widget& replay{
		_ui.emplace<text_widget>("replay", glm::vec2{4, 1000}, tr::align::BOTTOM_LEFT, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
	replay.unhide();
	widget& indicator{_ui.emplace<replay_playback_indicator_widget>("indicator", glm::vec2{992, 994}, tr::align::BOTTOM_RIGHT)};
	indicator.unhide();
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

std::unique_ptr<replay_state::state> replay_state::handle_event(const tr::event& event)
{
	if (event.type() == tr::key_down_event::ID && tr::key_down_event{event}.key == tr::keycode::ESCAPE) {
		return std::make_unique<replay_pause_state>(std::move(_game), true);
	}
	return nullptr;
}

std::unique_ptr<replay_state::state> replay_state::update(tr::duration)
{
	++_timer;
	switch (_substate) {
	case substate::WATCHING:
		if (tr::keyboard::held_mods() & tr::keymods::SHIFT) {
			if (_timer % 4 == 0) {
				_game->update();
			}
		}
		else if (tr::keyboard::held_mods() & tr::keymods::CTRL) {
			for (int i = 0; i < 4; ++i) {
				_game->update();
				if (_game->done()) {
					break;
				}
			}
		}
		else {
			_game->update();
		}

		if (_game->done()) {
			_substate = _game->game_over() ? substate::GAME_OVERING : substate::EXITING;
			_timer = 0;
		}
		else if (_timer % 120 == 60) {
			_ui.get("replay").hide();
		}
		else if (_timer % 120 == 0) {
			_ui.get("replay").unhide();
		}
		return nullptr;
	case substate::STARTING:
		if (_timer >= 0.5_s) {
			_substate = substate::WATCHING;
			_timer = 0;
		}
		return nullptr;
	case substate::GAME_OVERING:
		_game->update();
		if (_timer >= 0.75_s) {
			_substate = substate::EXITING;
			_timer = 0;
		}
		return nullptr;
	case substate::EXITING:
		return _timer >= 1_s ? std::make_unique<replays_state>() : nullptr;
	}
}

void replay_state::draw()
{
	_game->add_to_renderer();
	_ui.add_to_renderer();
	add_cursor_to_renderer(_game->cursor_pos());
	add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float replay_state::fade_overlay_opacity() const noexcept
{
	switch (_substate) {
	case substate::STARTING:
		return 1 - _timer / 0.5_sf;
	case substate::WATCHING:
	case substate::GAME_OVERING:
		return 0;
	case substate::EXITING:
		return std::max(static_cast<int>(_timer - 0.5_s), 0) / 0.5_sf;
	}
}

void replay_state::add_cursor_to_renderer(glm::vec2 pos) const
{
	tr::simple_color_mesh_ref quad{tr::renderer_2d::new_color_fan(layer::UI, 4)};
	fill_rotated_rect_vtx(quad.positions, pos, {6, 1}, {12, 2}, 45_degf);
	std::ranges::fill(quad.colors, color_cast<tr::rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1}));
	quad = tr::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(quad.positions, pos, {6, 1}, {12, 2}, -45_degf);
	std::ranges::fill(quad.colors, color_cast<tr::rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1}));
}