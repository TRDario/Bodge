#pragma once
#include "../gamemode.hpp"
#include "../replay.hpp"
#include "ball.hpp"
#include "player.hpp"

enum class game_type {
	REGULAR = 0x0,
	GAMEMODE_DESIGNER_TEST = 0x8,
	REPLAY = 0x10
};

/////////////////////////////////////////////////////////////////// GAME //////////////////////////////////////////////////////////////////

// Base game state.
class game {
  public:
	game(const gamemode& gamemode, std::uint64_t rng_seed);
	virtual ~game() = default;

	const gamemode& gamemode() const;
	bool game_over() const;
	ticks final_time() const;

	void update(const glm::vec2& input);
	virtual void update();

	void add_to_renderer() const;

  private:
	::gamemode m_gamemode;
	tr::xorshiftr_128p m_rng;
	std::optional<::player> m_player;
	tr::static_vector<ball, 255> m_balls;
	float m_next_ball_size;
	float m_next_ball_velocity;
	ticks m_time_since_start;
	ticks m_time_since_last_spawn;

	void add_new_ball();
	void add_overlay_to_renderer() const;
	void add_border_to_renderer() const;
};

/////////////////////////////////////////////////////////////// ACTIVE GAME ///////////////////////////////////////////////////////////////

// Game that is actively being played.
class active_game : public game {
  public:
	active_game(const ::gamemode& gamemode, std::uint64_t seed = engine::rng.generate<std::uint64_t>());

	replay& replay();

	void update() override;

  private:
	class replay m_replay;

	using game::update;
};

/////////////////////////////////////////////////////////////// REPLAY GAME ///////////////////////////////////////////////////////////////

// Game that is being played back through a replay.
class replay_game : public game {
  public:
	replay_game(const struct gamemode& gamemode, replay&& replay);
	replay_game(const replay_game& r);

	bool done() const;
	glm::vec2 cursor_pos() const;

	void update() override;

  private:
	replay m_replay;

	using game::update;
};