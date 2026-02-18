///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides game simulation classes.                                                                                                     //
//                                                                                                                                       //
// Game hierarchy:                                                                                                                       //
// • playerless_game     - No player object or most mechanics, only the balls, used for menu backgrounds.                                //
//     • game            - Implements the player and mechanics, undefined input method.                                                  //
//         • active_game - Input is taken from the mouse.                                                                                //
//         • replay_game - Input is taken from a replay file.                                                                            //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "game/ball.hpp"
#include "game/life_fragment.hpp"
#include "game/player.hpp"
#include "gamemode.hpp"
#include "replay.hpp"

///////////////////////////////////////////////////////////// PLAYERLESS_GAME /////////////////////////////////////////////////////////////

// Base game state (no player).
class playerless_game {
  public:
	// Creates a new game.
	playerless_game(gamemode gamemode, u64 rng_seed);

	// Gets the gamemode of the game.
	const gamemode& gamemode() const;

	// Updates the game state.
	void tick();

	// Adds the game to the renderer.
	void add_to_renderer() const;

  protected:
	// The gamemode of the game.
	const ::gamemode m_gamemode;
	// Random number generator for gameplay.
	tr::xorshiftr_128p m_rng;
	// List of balls.
	tr::static_vector<ball, 255> m_balls;
	// Elapsed time since the game began.
	ticks m_elapsed_time;

  private:
	// Elapsed time since the last ball was spawned.
	ticks m_time_since_last_ball;
	// Radius of the next spawned ball.
	float m_next_ball_size;
	// Velocity of the next spawned ball.
	float m_next_ball_velocity;

	// Adds a new ball to the game.
	void add_new_ball();

	// Adds the ball trail overlay to the renderer.
	void add_ball_trail_overlay_to_renderer() const;
	// Adds the field border to the renderer.
	void add_border_to_renderer() const;
};

////////////////////////////////////////////////////////////////// GAME ///////////////////////////////////////////////////////////////////

// Base game state (with a player).
class game : private playerless_game {
  public:
	// Creates a new game.
	game(::gamemode gamemode, u64 rng_seed);
	// Virtual destructor.
	virtual ~game() = default;

	// Gets whether the game is over.
	bool game_over() const;
	// Gets the final achieved score.
	i64 final_score() const;
	// Gets the final achieved time.
	ticks final_time() const;
	// Gets the gamemode of the game.
	using playerless_game::gamemode;

	// Updates the game.
	virtual void tick() = 0;

	// Adds the game to the renderer.
	void add_to_renderer() const;

  protected:
	// Base update function taking in a player input.
	void tick(const glm::vec2& input);

  private:
	// Information needed for rendering the timer display.
	struct timer_render_info {
		// The current time.
		ticks time;
		// The tint of the timer display.
		tr::rgba8 tint;
		// The scale of the timer display.
		float scale;
	};
	// Information needed for rendering the score display.
	struct score_render_info {
		// The tint of the score display.
		tr::rgba8 tint;
		// The scale of the score display.
		float scale;
	};

	// Atlas holding the digits used by the timer and score displays.
	mutable std::variant<tr::gfx::bitmap_atlas<char>, tr::gfx::dyn_atlas<char>> m_number_atlas;
	// Player state.
	player m_player;
	// List of collectible life fragments.
	std::array<life_fragment, 9> m_life_fragments{};
	// Fragments used to draw the animation of a life shattering after getting hit.
	std::array<fragment, 6> m_shattered_life_fragments;
	// Number of remaining lives.
	int m_lives_left;
	// Current achieved score.
	i64 m_score;
	// Timer measuring the elapsed time since game over.
	startable_timer m_game_over_timer;
	// Timer controlling the 1-UP animation.
	decrementing_timer<0.2_s> m_1up_animation_timer;
	// Timer controlling the hit animation.
	decrementing_timer<0.2_s> m_hit_animation_timer;
	// Timer controlling the screen shake animation
	decrementing_timer<0.67_s> m_screen_shake_timer;
	// Timer controlling the sytle cooldown.
	decrementing_timer<0.1_s> m_style_cooldown_timer;
	// Timer controlling the score display animation,
	decrementing_timer<0.1_s> m_score_animation_timer;
	// Timer counting the time spent in the center region.
	accumulating_timer<3_s> m_center_timer;
	// Timer counting the time spent in an edge region.
	accumulating_timer<3_s> m_edge_timer;
	// Timer counting the time spent in a corner region.
	accumulating_timer<3_s> m_corner_timer;
	// Timer controlling the lives display hiding animation.
	accumulating_timer<0.25_s> m_lives_hover_timer;
	// Timer controlling the timer display hiding animation.
	accumulating_timer<0.25_s> m_timer_hover_timer;
	// Timer controlling the score display hiding animation.
	accumulating_timer<0.25_s> m_score_hover_timer;
	// Flag denoting whether the next second tick sound should be a deeper "tock".
	bool m_tock;

	// Gets the size of a string of text.
	glm::vec2 text_size(const std::string& text, float scale) const;
	// Gets information needed for rendering the timer display.
	timer_render_info timer_render_info() const;
	// Gets information needed for rendering the score display.
	score_render_info score_render_info() const;

	// Plays the tick second on second marks.
	void play_tick_sound_if_needed();
	// Updates the various game timers.
	void update_timers();
	// Updates the collectible life fragments.
	void update_life_fragments();
	// Checks if the player is hovering over the timer display and increments or decrements the related timer based on the result.
	void check_if_timer_obstructed();
	// Checks if the player is hovering over the lives display and increments or decrements the related timer based on the result.
	void check_if_lives_obstructed();
	// Checks if the player is hovering over the score display and increments or decrements the related timer based on the result.
	void check_if_score_obstructed();
	// Checks for and handles the player getting hit.
	void check_if_player_was_hit();
	// Sets up the fragments used for the shattered life animation.
	void set_up_shattered_life_fragments();
	// Checks for and handles the player collecting life fragments.
	void check_if_player_collected_life_fragments();
	// Adds to the score.
	void add_to_score(i64 change);
	// Checks for and applies score ticks.
	void check_for_score_ticks();
	// Determines whether the player is in a ball's style region.
	bool player_in_ball_style_region(const ball& ball, float ball_velocity) const;
	// Checks for and applies style points.
	void check_for_style_points();
	// Applies screenshake.
	void set_screen_shake() const;

	// Adds the timer display to the renderer.
	void add_timer_to_renderer() const;
	// Adds the lives display to the renderer.
	void add_lives_to_renderer() const;
	// Adds an appearing life from the lives display to the renderer.
	void add_appearing_life_to_renderer(tr::rgb8 color, u8 base_opacity) const;
	// Adds a shattering life from the lives display to the renderer.
	void add_shattering_life_to_renderer(tr::rgb8 color, u8 base_opacity) const;
	// Adds the score display to the renderer.
	void add_score_to_renderer() const;
};

/////////////////////////////////////////////////////////////// ACTIVE GAME ///////////////////////////////////////////////////////////////

// Game that is actively being played.
class active_game : public game {
  public:
	// Creates a new active game.
	active_game(::gamemode gamemode, u64 seed = g_rng.generate<u64>());

	// Updates the game state.
	void tick() override;

	// Replay recorded of the game.
	replay replay;
};

/////////////////////////////////////////////////////////////// REPLAY GAME ///////////////////////////////////////////////////////////////

// Game that is being played back through a replay.
class replay_game : public game {
  public:
	// Creates a replay game from a replay.
	replay_game(replay&& replay);
	// Creates a replay game using the same replay as an existing replay game.
	replay_game(const replay_game& r);

	// Gets whether the replay is done playing.
	bool done() const;
	// Gets the current position of the replay's mouse position.
	glm::vec2 cursor_pos() const;

	// Updates the game state.
	void tick() override;

  private:
	// The replay being read from.
	replay m_replay;
};