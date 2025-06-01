#include "../../include/engine.hpp"
#include "../../include/state/game_state.hpp"
// #include "../../include/game/game_over_state.hpp"
#include "../../include/state/pause_state.hpp"

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

game_state::game_state(unique_ptr<active_game>&& game, bool fade_in) noexcept
	: _substate{fade_in ? substate::STARTING : substate::PLAYING}, _timer{0}, _game{std::move(game)}
{
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

u32 game_state::type() const noexcept
{
	return ID;
}

unique_ptr<state> game_state::handle_event(const tr::event& event)
{
	if (event.type() == tr::key_down_event::ID && tr::key_down_event{event}.key == key::ESCAPE) {
		return make_unique<pause_state>(std::move(_game), true);
	}
	return nullptr;
}

unique_ptr<state> game_state::update(tr::duration)
{
	++_timer;
	switch (_substate) {
	case substate::PLAYING:
		_game->update();
		if (_game->game_over()) {
			// return make_unique<game_over_state>(std::move(_game));
		}
		break;
	case substate::STARTING:
		if (_timer >= 0.5_s) {
			_substate = substate::PLAYING;
			_timer = 0;
		}
	}
	return nullptr;
}

void game_state::draw()
{
	_game->add_to_renderer();
	add_fade_overlay_to_renderer(_substate == substate::STARTING ? 1 - (_timer / static_cast<float>(0.5_s)) : 0);
	engine::layered_renderer().draw(engine::screen());
}