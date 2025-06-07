#include "../../include/engine.hpp"
#include "../../include/state/replay_pause_state.hpp"
#include "../../include/state/replay_state.hpp"
#include "../../include/state/replays_state.hpp"

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

replay_state::replay_state(unique_ptr<replay_game>&& game, bool fade_in) noexcept
	: _substate{fade_in ? substate::STARTING : substate::WATCHING}, _timer{0}, _game{std::move(game)}
{
	widget& replay{_ui.emplace<text_widget>("replay", vec2{4, 1000}, BOTTOM_LEFT, font::LANGUAGE, ttf_style::NORMAL, 48)};
	replay.unhide();
	replay_playback_indicator_widget& indicator{_ui.emplace<replay_playback_indicator_widget>("indicator", vec2{992, 994}, BOTTOM_RIGHT)};
	indicator.unhide();
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

u32 replay_state::type() const noexcept
{
	return ID;
}

unique_ptr<replay_state::state> replay_state::handle_event(const tr::event& event)
{
	if (event.type() == tr::key_down_event::ID && tr::key_down_event{event}.key == key::ESCAPE) {
		return make_unique<replay_pause_state>(std::move(_game), true);
	}
	return nullptr;
}

unique_ptr<replay_state::state> replay_state::update(tr::duration)
{
	++_timer;
	switch (_substate) {
	case substate::WATCHING:
		if (tr::keyboard::held_mods() & mods::SHIFT) {
			if (_timer % 4 == 0) {
				_game->update();
			}
		}
		else if (tr::keyboard::held_mods() & mods::CTRL) {
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
			_substate = substate::EXITING;
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
	case substate::EXITING:
		return _timer >= 1_s ? make_unique<replays_state>() : nullptr;
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
		return 0;
	case substate::EXITING:
		return max(static_cast<int>(_timer - 0.5_s), 0) / 0.5_sf;
	}
}

void replay_state::add_cursor_to_renderer(vec2 pos) const
{
	color_alloc quad{tr::renderer_2d::new_color_fan(layer::CURSOR, 4)};
	fill_rotated_rect_vtx(quad.positions, pos, {6, 1}, {12, 2}, 45_degf);
	rs::fill(quad.colors, color_cast<rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1}));
	quad = tr::renderer_2d::new_color_fan(layer::CURSOR, 4);
	fill_rotated_rect_vtx(quad.positions, pos, {6, 1}, {12, 2}, -45_degf);
	rs::fill(quad.colors, color_cast<rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1}));
}