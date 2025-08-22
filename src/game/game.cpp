#include "../../include/game/game.hpp"
#include "../../include/graphics.hpp"
#include "../../include/score.hpp"
#include "../../include/system.hpp"

////////////////////////////////////////////////////////////////// GAME ///////////////////////////////////////////////////////////////////

game::game(const ::gamemode& gamemode, std::uint64_t rng_seed)
	: m_gamemode{gamemode}
	, m_rng{rng_seed}
	, m_next_ball_size{gamemode.ball.initial_size}
	, m_next_ball_velocity{gamemode.ball.initial_velocity}
	, m_time_since_start{0}
	, m_time_since_last_spawn{0}
{
	if (!m_gamemode.player.is_autoplay()) {
		m_player.emplace(gamemode.player, engine::scorefile.personal_best(m_gamemode));
	}

	for (int i = 0; i < gamemode.ball.starting_count; ++i) {
		add_new_ball();
	}
}

//

const gamemode& game::gamemode() const
{
	return m_gamemode;
}

bool game::game_over() const
{
	return m_player->game_over();
}

ticks game::final_time() const
{
	return m_time_since_start - m_player->time_since_game_over();
}

//

void game::update(const glm::vec2& input)
{
	++m_time_since_start;
	++m_time_since_last_spawn;

	if (m_time_since_last_spawn >= m_gamemode.ball.spawn_interval && m_balls.size() < m_gamemode.ball.max_count) {
		add_new_ball();
	}

	for (std::uint8_t i = 0; i < m_balls.size(); ++i) {
		m_balls[i].update();
		if (m_balls[i].tangible()) {
			for (std::uint8_t j = i + 1; j < m_balls.size(); ++j) {
				if (m_balls[j].tangible() && colliding(m_balls[i], m_balls[j])) {
					handle_collision(m_balls[i], m_balls[j]);
				}
			}
		}
	}

	if (m_player.has_value()) {
		if (m_player->game_over()) {
			m_player->update();
		}
		else {
			m_player->update(input);
			if (m_player->colliding_with_any_of(m_balls)) {
				m_player->hit();
			}
		}
	}
}

void game::update()
{
	update({});
}

void game::add_to_renderer() const
{
	if (m_player.has_value()) {
		m_player->add_to_renderer();
	}
	std::ranges::for_each(m_balls, &ball::add_to_renderer);
	add_overlay_to_renderer();
	add_border_to_renderer();
}

//

void game::add_new_ball()
{
	m_time_since_last_spawn = 0;
	m_balls.emplace_back(m_rng, m_next_ball_size, m_next_ball_velocity);
	m_next_ball_size = std::min(m_next_ball_size + m_gamemode.ball.size_step, 250.0f);
	m_next_ball_velocity = std::min(m_next_ball_velocity + m_gamemode.ball.velocity_step, 5000.0f);
}

void game::add_overlay_to_renderer() const
{
	tr::gfx::simple_color_mesh_ref overlay{tr::gfx::renderer_2d::new_color_fan(layer::BALL_TRAILS_OVERLAY, 4)};
	std::ranges::copy(OVERLAY_POSITIONS, overlay.positions.begin());
	std::ranges::fill(overlay.colors, "00000000"_rgba8);
}

void game::add_border_to_renderer() const
{
	tr::gfx::simple_color_mesh_ref border{tr::gfx::renderer_2d::new_color_outline(layer::BORDER, 4)};
	tr::fill_rect_outline_vtx(border.positions, {{2, 2}, {996, 996}}, 4);
	std::ranges::fill(border.colors, color_cast<tr::rgba8>(tr::hsv{float(engine::settings.secondary_hue), 1, 1}));
}

/////////////////////////////////////////////////////////////// ACTIVE GAME ///////////////////////////////////////////////////////////////

active_game::active_game(const ::gamemode& gamemode, std::uint64_t seed)
	: game{gamemode, seed}, m_replay{gamemode, seed}
{
}

//

replay& active_game::replay()
{
	return m_replay;
}

//

void active_game::update()
{
	const bool is_game_over{game_over()};
	const glm::vec2 input{engine::mouse_pos()};
	update(input);
	if (!is_game_over) {
		m_replay.append(input);
	}
}

////////////////////////////////////////////////////////////// REPLAY GAME ////////////////////////////////////////////////////////////////

replay_game::replay_game(const ::gamemode& gamemode, replay&& replay)
	: game{gamemode, replay.header().seed}, m_replay{std::move(replay)}
{
}

replay_game::replay_game(const replay_game& r)
	: replay_game{r.gamemode(), replay{r.m_replay}}
{
}

//

bool replay_game::done() const
{
	return m_replay.done();
}

glm::vec2 replay_game::cursor_pos() const
{
	return m_replay.prev_input();
}

//

void replay_game::update()
{
	update(done() ? m_replay.prev_input() : m_replay.next_input());
}