#pragma once
#include "../gamemode.hpp"
#include "../replay.hpp"
#include "ball.hpp"
#include "player.hpp"

// Types of games.
enum class game_type {
	// Regular game.
	REGULAR = 0x0,
	// Gamemode designer test game.
	TEST = 0x8,
	// Replay.
	REPLAY = 0x10
};

// Game state.
class game {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a new game state.
	game(const gamemode& gamemode, std::uint64_t rng_seed);
	game(game&&) noexcept = default;
	virtual ~game() noexcept = default;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Gets the game's gamemode.
	const gamemode& gamemode() const noexcept;
	// Gets whether the game is over.
	bool game_over() const noexcept;
	// Gets the elapsed in-game time.
	ticks result() const noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Updates the game state given an input.
	void update(const glm::vec2& input) noexcept;
	// Virtual update function.
	virtual void update();

	////////////////////////////////////////////////////////////// GRAPHICS ///////////////////////////////////////////////////////////////

	// Adds the game graphics to the renderer drawing list.
	void add_to_renderer() const;

  private:
	// The gamemode being used.
	::gamemode _gamemode;
	// Game RNG.
	tr::xorshiftr_128p _rng;
	// An std::optional player object.
	std::optional<::player> _player;
	// A vector of ball objects.
	tr::static_vector<ball, 255> _balls;
	// The size of the next spawned ball.
	float _ball_size;
	// The velocity of the next spawned ball.
	float _ball_velocity;
	// The amount of elapsed ticks since game start.
	ticks _age;
	// The amount of elapsed ticks since the last ball spawn.
	ticks _last_spawn;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Adds a new ball to the game field.
	void add_new_ball() noexcept;
	// Adds the border mesh to the renderer.
	void add_overlay_to_renderer() const;
	// Adds the border mesh to the renderer.
	void add_border_to_renderer() const;
};

// Game that is currently being played.
class active_game : public game {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a new game.
	active_game(const ::gamemode& gamemode, std::uint64_t seed = rng.generate<std::uint64_t>());

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Gets the game's replay.
	replay& replay() noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Updates the game state.
	void update() override;

  private:
	// The replay storing inputs.
	::replay _replay;

	using game::update;
};

// Game that is being played back through a replay.
class replay_game : public game {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a replay game.
	replay_game(const ::gamemode& gamemode, replay&& replay);
	// Copies a replay game.
	replay_game(const replay_game& r);
	// Moves a replay game.
	replay_game(replay_game&&) noexcept = default;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Gets whether the replay is done.
	bool done() const noexcept;
	// Gets the current position of the replay cursor.
	glm::vec2 cursor_pos() const noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Updates the game state.
	void update() noexcept override;

  private:
	// The replay from which inputs are gotten.
	replay _replay;

	using game::update;
};