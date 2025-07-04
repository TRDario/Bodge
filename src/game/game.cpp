#include "../../include/engine.hpp"
#include "../../include/game/game.hpp"
#include "../../include/score.hpp"

////////////////////////////////////////////////////////////////// GAME ///////////////////////////////////////////////////////////////////

game::game(const ::gamemode& gamemode, std::uint64_t rng_seed)
	: _gamemode{gamemode}
	, _rng{rng_seed}
	, _ball_size{gamemode.ball.initial_size}
	, _ball_velocity{gamemode.ball.initial_velocity}
	, _age{0}
	, _last_spawn{0}
{
	if (!_gamemode.player.autoplay()) {
		_player.emplace(gamemode.player, scorefile.pb(_gamemode));
	}

	for (int i = 0; i < gamemode.ball.starting_count; ++i) {
		add_new_ball();
	}
}

const gamemode& game::gamemode() const noexcept
{
	return _gamemode;
}

bool game::game_over() const noexcept
{
	return _player->game_over();
}

ticks game::result() const noexcept
{
	return _age - _player->time_since_game_over();
}

void game::update(const glm::vec2& input) noexcept
{
	++_age;
	++_last_spawn;

	if (_last_spawn >= _gamemode.ball.spawn_interval && _balls.size() < _gamemode.ball.max_count) {
		add_new_ball();
	}

	for (std::size_t i = 0; i < _balls.size(); ++i) {
		_balls[i].update();

		if (_balls[i].tangible()) {
			for (std::size_t j = i + 1; j < _balls.size(); ++j) {
				if (_balls[j].tangible() && colliding(_balls[i], _balls[j])) {
					handle_collision(_balls[i], _balls[j]);
				}
			}
		}
	}

	if (_player.has_value()) {
		if (_player->game_over()) {
			_player->update();
		}
		else {
			_player->update(input);
			if (colliding(*_player, _balls)) {
				_player->hit();
			}
		}
	}
}

void game::update()
{
	update({});
}

void game::add_overlay_to_renderer() const
{
	tr::simple_color_mesh_ref overlay{tr::renderer_2d::new_color_fan(layer::BALL_TRAILS_OVERLAY, 4)};
	std::ranges::copy(OVERLAY_POSITIONS, overlay.positions.begin());
	std::ranges::fill(overlay.colors, "00000000"_rgba8);
}

void game::add_border_to_renderer() const
{
	tr::simple_color_mesh_ref border{tr::renderer_2d::new_color_outline(layer::BORDER, 4)};
	tr::fill_rect_outline_vtx(border.positions, {{2, 2}, {996, 996}}, 4);
	std::ranges::fill(border.colors, color_cast<tr::rgba8>(tr::hsv{static_cast<float>(settings.secondary_hue), 1, 1}));
}

void game::add_to_renderer() const
{
	if (_player.has_value()) {
		_player->add_to_renderer();
	}
	std::ranges::for_each(_balls, &ball::add_to_renderer);
	add_overlay_to_renderer();
	add_border_to_renderer();
}

void game::add_new_ball() noexcept
{
	_last_spawn = 0;
	_balls.emplace_back(_rng, _ball_size, _ball_velocity);
	_ball_size = std::min(_ball_size + _gamemode.ball.size_step, 250.0f);
	_ball_velocity = std::min(_ball_velocity + _gamemode.ball.velocity_step, 5000.0f);
}

/////////////////////////////////////////////////////////////// ACTIVE_GAME ///////////////////////////////////////////////////////////////

active_game::active_game(const ::gamemode& gamemode, std::uint64_t seed)
	: game{gamemode, seed}, _replay{gamemode, seed}
{
}

replay& active_game::replay() noexcept
{
	return _replay;
}

void active_game::update()
{
	const bool game_over{this->game_over()};
	const glm::vec2 input{engine::mouse_pos()};
	update(input);
	if (!game_over) {
		_replay.append(input);
	}
}

////////////////////////////////////////////////////////////// REPLAY_GAME ////////////////////////////////////////////////////////////////

replay_game::replay_game(const ::gamemode& gamemode, replay&& replay)
	: game{gamemode, replay.header().seed}, _replay{std::move(replay)}
{
}

replay_game::replay_game(const replay_game& r)
	: replay_game{r.gamemode(), replay{r._replay}}
{
}

bool replay_game::done() const noexcept
{
	return _replay.done();
}

glm::vec2 replay_game::cursor_pos() const noexcept
{
	return _replay.current();
}

void replay_game::update() noexcept
{
	update(done() ? _replay.current() : _replay.next());
}