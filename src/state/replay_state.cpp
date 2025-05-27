#include "../../include/engine.hpp"
#include "../../include/state/replay_state.hpp"
#include "../../include/state/replays_state.hpp"

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

replay_state::replay_state(replay_game&& game, bool fade_in) noexcept
	: _substate{fade_in ? substate::STARTING : substate::WATCHING}, _timer{0}, _game{std::move(game)}
{
	basic_text_widget& replay{_ui.emplace<basic_text_widget>("replay", vec2{4, 1000}, BOTTOM_LEFT, font::LANGUAGE, 48)};
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
		// return make_unique<pause_state>(std::move(_game), true);
	}
	return nullptr;
}

unique_ptr<replay_state::state> replay_state::update(tr::duration)
{
	++_timer;
	switch (_substate) {
	case substate::WATCHING:
		if (keyboard::held_mods() & mods::SHIFT) {
			if (_timer % 4 == 0) {
				_game.update();
			}
		}
		else if (keyboard::held_mods() & mods::CTRL) {
			for (int i = 0; i < 4; ++i) {
				_game.update();
				if (_game.done()) {
					break;
				}
			}
		}
		else {
			_game.update();
		}

		if (_game.done()) {
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
	_game.add_to_renderer();
	_ui.add_to_renderer();
	add_cursor_to_renderer(_game.cursor_pos());
	add_fade_overlay_to_renderer(fade_overlay_opacity());

	engine::layered_renderer().draw_up_to_layer(layer::UI, engine::screen());
	engine::batched_renderer().draw(engine::screen());
	engine::layered_renderer().draw(engine::screen());
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

void replay_state::add_cursor_to_renderer(glm::vec2 pos) const
{
	vector<u16> indices(2 * poly_idx(4));
	fill_poly_idx(indices.begin(), 4, 0);
	fill_poly_idx(indices.begin() + poly_idx(4), 4, 4);

	array<clrvtx, 8> vtx;
	rs::fill(colors(vtx), color_cast<rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1}));
	tr::fill_rotated_rect_vtx(positions(vtx).begin(), pos, {6, 1}, {12, 2}, 45_degf);
	tr::fill_rotated_rect_vtx(positions(vtx).begin() + 4, pos, {6, 1}, {12, 2}, -45_degf);
	engine::layered_renderer().add_color_mesh(layer::CURSOR, vtx, std::move(indices));
	engine::layered_renderer().draw(engine::screen());
}