#include "../../include/engine.hpp"
#include "../../include/state/game_over_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/pause_state.hpp"

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

game_state::game_state(std::unique_ptr<active_game>&& game, bool fade_in) noexcept
	: _substate{fade_in ? substate::STARTING : substate::PLAYING}, _timer{0}, _game{std::move(game)}
{
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> game_state::handle_event(const tr::event& event)
{
	if (event.type() == tr::key_down_event::ID && tr::key_down_event{event}.key == tr::keycode::ESCAPE) {
		return std::make_unique<pause_state>(std::move(_game), engine::mouse_pos(), true);
	}
	return nullptr;
}

std::unique_ptr<tr::state> game_state::update(tr::duration)
{
	++_timer;
	switch (_substate) {
	case substate::STARTING:
		if (_timer >= 0.5_s) {
			_substate = substate::PLAYING;
			_timer = 0;
		}
		return nullptr;
	case substate::PLAYING:
		_game->update();
		if (_game->game_over()) {
			_substate = substate::GAME_OVER;
			_timer = 0;
		}
		return nullptr;
	case substate::GAME_OVER:
		_game->update();
		return _timer >= 0.75_s ? std::make_unique<game_over_state>(std::move(_game), true) : nullptr;
	}
}

void game_state::draw()
{
	_game->add_to_renderer();
	add_fade_overlay_to_renderer(_substate == substate::STARTING ? 1 - (_timer / static_cast<float>(0.5_s)) : 0);
	tr::renderer_2d::draw(engine::screen());
}