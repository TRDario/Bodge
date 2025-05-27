#include "../../include/game/game.hpp"
#include "../../include/engine.hpp"
#include "../../include/score.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr array<clrvtx, 4> BALL_TRAILS_OVERLAY_QUAD{{
	{{0, 0}, {0, 0, 0, 0}},
	{{1000, 0}, {0, 0, 0, 0}},
	{{1000, 1000}, {0, 0, 0, 0}},
	{{0, 1000}, {0, 0, 0, 0}},
}};

////////////////////////////////////////////////////////////////// GAME ///////////////////////////////////////////////////////////////////

game::game(const ::gamemode& gamemode, std::uint64_t rng_seed)
	: _gamemode{gamemode}
	, _rng{rng_seed}
	, _ball_size{gamemode.ball.starting_size}
	, _ball_velocity{gamemode.ball.starting_velocity}
	, _age{0}
	, _last_spawn{0}
{
	if (!_gamemode.player.autoplay()) {
		const ticks pb{scorefile.scores.contains(gamemode) ? scorefile.scores.at(gamemode).front().result : 0};
		_player.emplace(gamemode.player, pb);
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

ticks game::timestamp() const noexcept
{
	return _age;
}

void game::update(const vec2& input) noexcept
{
	++_age;
	++_last_spawn;

	if (_last_spawn >= _gamemode.ball.spawn_interval && _balls.size() < _gamemode.ball.max_count) {
		add_new_ball();
	}

	for (size_t i = 0; i < _balls.size(); ++i) {
		_balls[i].update();

		if (_balls[i].tangible()) {
			for (size_t j = i + 1; j < _balls.size(); ++j) {
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

void game::add_border_mesh() const
{
	vector<u16> border_indices;
	border_indices.resize(poly_outline_idx(4));
	fill_poly_outline_idx(border_indices, 4, 0);

	array<clrvtx, 8> border_mesh;
	fill_rect_outline_vtx(positions(border_mesh), {{2, 2}, {996, 996}}, 4);
	rs::fill(colors(border_mesh), color_cast<rgba8>(tr::hsv{static_cast<float>(settings.secondary_hue), 1, 1}));

	engine::layered_renderer().add_color_mesh(layer::BORDER, border_mesh, std::move(border_indices));
}

void game::add_to_renderer() const
{
	if (_player.has_value()) {
		_player->add_to_renderer();
	}
	rs::for_each(_balls, &ball::add_to_renderer);
	engine::layered_renderer().add_color_quad(layer::BALL_TRAILS_OVERLAY, BALL_TRAILS_OVERLAY_QUAD);
	add_border_mesh();
}

void game::add_new_ball() noexcept
{
	_last_spawn = 0;
	_balls.emplace_back(_rng, _ball_size, _ball_velocity);
	_ball_size = min(_ball_size + _gamemode.ball.size_step, 250.0f);
	_ball_velocity = min(_ball_velocity + _gamemode.ball.velocity_step, 5000.0f);
}

/////////////////////////////////////////////////////////////// ACTIVE_GAME ///////////////////////////////////////////////////////////////

active_game::active_game(const ::gamemode& gamemode, std::uint64_t seed)
	: game{gamemode, seed}, _replay{gamemode, seed}
{
}

void active_game::update()
{
	const vec2 input{engine::mouse_pos()};
	update(input);
	_replay.append(input);
}

////////////////////////////////////////////////////////////// REPLAY_GAME ////////////////////////////////////////////////////////////////

replay_game::replay_game(const ::gamemode& gamemode, replay&& replay)
	: game{gamemode, replay.header().seed}, _replay{std::move(replay)}
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
	update(_replay.next());
}