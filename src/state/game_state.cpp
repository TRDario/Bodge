#include "../../include/state/game_state.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/game_over_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/pause_state.hpp"
#include "../../include/state/replays_state.hpp"

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

game_state::game_state(std::unique_ptr<game>&& game, game_type type, bool fade_in) noexcept
	: _substate{(fade_in ? substate_base::STARTING : substate_base::ONGOING) | type}, _timer{0}, _game{std::move(game)}
{
	if (type == game_type::REPLAY) {
		widget& replay{
			_ui.emplace<text_widget>("replay", glm::vec2{4, 1000}, tr::align::BOTTOM_LEFT, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
		replay.unhide();
		widget& indicator{_ui.emplace<replay_playback_indicator_widget>("indicator", glm::vec2{992, 994}, tr::align::BOTTOM_RIGHT)};
		indicator.unhide();
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> game_state::handle_event(const tr::event& event)
{
	if (event.type() == tr::key_down_event::ID && tr::key_down_event{event}.key == tr::keycode::ESCAPE) {
		audio::play(sfx::PAUSE, 0.8f, 0.0f);
		return std::make_unique<pause_state>(std::move(_game), to_type(_substate), engine::mouse_pos(), true);
	}
	return nullptr;
}

std::unique_ptr<tr::state> game_state::update(tr::duration)
{
	++_timer;
	_ui.update();
	switch (to_base(_substate)) {
	case substate_base::STARTING:
		if (_timer >= 0.5_s) {
			_substate = substate_base::ONGOING | to_type(_substate);
			_timer = 0;
		}
		return nullptr;
	case substate_base::ONGOING:
		if (to_type(_substate) == game_type::REPLAY) {
			if (engine::held_keymods() & tr::keymod::SHIFT) {
				if (_timer % 4 == 0) {
					_game->update();
				}
			}
			else if (engine::held_keymods() & tr::keymod::CTRL) {
				for (int i = 0; i < 4; ++i) {
					_game->update();
					if (static_cast<replay_game*>(_game.get())->done()) {
						break;
					}
				}
			}
			else {
				_game->update();
			}

			if (static_cast<replay_game*>(_game.get())->done()) {
				_substate = (_game->game_over() ? substate_base::GAME_OVER : substate_base::EXITING) | game_type::REPLAY;
				_timer = 0;
			}
			else if (_timer % 120 == 60) {
				_ui.get("replay").hide();
			}
			else if (_timer % 120 == 0) {
				_ui.get("replay").unhide();
			}
		}
		else {
			_game->update();
			if (_game->game_over()) {
				_substate = substate_base::GAME_OVER | to_type(_substate);
				_timer = 0;
			}
		}
		return nullptr;
	case substate_base::GAME_OVER:
		_game->update();
		if (_timer >= 0.75_s) {
			tr::renderer_2d::set_default_transform(TRANSFORM);
			switch (to_type(_substate)) {
			case game_type::REGULAR: {
				const ticks prev_pb{scorefile.category_pb(_game->gamemode())};
				scorefile.update_category(_game->gamemode(), _game->result());
				return std::make_unique<game_over_state>(std::unique_ptr<active_game>{static_cast<active_game*>(_game.release())}, true,
														 prev_pb);
			}
			case game_type::TEST:
			case game_type::REPLAY:
				_substate = substate_base::EXITING | to_type(_substate);
				_timer = 0;
				break;
			}
		}
		return nullptr;
	case substate_base::EXITING:
		if (_timer >= 1_s) {
			tr::renderer_2d::set_default_transform(TRANSFORM);
			if (to_type(_substate) == game_type::REPLAY) {
				return std::make_unique<replays_state>();
			}
			else {
				return std::make_unique<gamemode_designer_state>(_game->gamemode());
			}
		}
		return nullptr;
	}
}

void game_state::draw()
{
	_game->add_to_renderer();
	if (to_type(_substate) == game_type::REPLAY) {
		_ui.add_to_renderer();
		add_replay_cursor_to_renderer(static_cast<replay_game*>(_game.get())->cursor_pos());
	}
	add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

game_state::substate operator|(const game_state::substate_base& l, const game_type& r) noexcept
{
	return static_cast<game_state::substate>(static_cast<int>(l) | static_cast<int>(r));
}

game_state::substate_base to_base(game_state::substate state) noexcept
{
	return static_cast<game_state::substate_base>(static_cast<int>(state) & 0x3);
}

game_type to_type(game_state::substate state) noexcept
{
	return static_cast<game_type>(static_cast<int>(state) & 0x18);
}

float game_state::fade_overlay_opacity() const noexcept
{
	switch (to_base(_substate)) {
	case substate_base::STARTING:
		return 1 - _timer / 0.5_sf;
	case substate_base::ONGOING:
	case substate_base::GAME_OVER:
		return 0;
	case substate_base::EXITING:
		return std::max(static_cast<int>(_timer - 0.5_s), 0) / 0.5_sf;
	}
}

void game_state::add_replay_cursor_to_renderer(glm::vec2 pos) const
{
	tr::simple_color_mesh_ref quad{tr::renderer_2d::new_color_fan(layer::UI, 4)};
	fill_rotated_rect_vtx(quad.positions, pos, {6, 1}, {12, 2}, 45_degf);
	std::ranges::fill(quad.colors, color_cast<tr::rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1}));
	quad = tr::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(quad.positions, pos, {6, 1}, {12, 2}, -45_degf);
	std::ranges::fill(quad.colors, color_cast<tr::rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1}));
}