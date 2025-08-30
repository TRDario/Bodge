#pragma once
#include "../gamemode.hpp"
#include "../replay.hpp"
#include "ball.hpp"
#include "life_fragment.hpp"
#include "player.hpp"

/////////////////////////////////////////////////////////////////// GAME //////////////////////////////////////////////////////////////////

// Base game state (no player).
class playerless_game {
  public:
	playerless_game(const gamemode& gamemode, u64 rng_seed);

	const gamemode& gamemode() const;

	void update();

	void add_to_renderer() const;

  protected:
	::gamemode m_gamemode;
	tr::xorshiftr_128p m_rng;
	tr::static_vector<ball, 255> m_balls;
	ticks m_time_since_start;

  private:
	ticks m_time_since_last_ball;
	float m_next_ball_size;
	float m_next_ball_velocity;

	void add_new_ball();
	void add_overlay_to_renderer() const;
	void add_border_to_renderer() const;
};

/////////////////////////////////////////////////////////////// PLAYER GAME ///////////////////////////////////////////////////////////////

enum class game_type {
	REGULAR = 0x0,
	GAMEMODE_DESIGNER_TEST = 0x8,
	REPLAY = 0x10
};

// Base game state (with a player).
class game : private playerless_game {
  public:
	game(const ::gamemode& gamemode, u64 rng_seed);
	virtual ~game() = default;

	bool game_over() const;
	ticks final_time() const;
	using playerless_game::gamemode;

	virtual void update() = 0;

	void add_to_renderer() const;

  protected:
	void update(const glm::vec2& input);

  private:
	tr::gfx::dyn_atlas<char> m_timer_atlas;
	player m_player;
	tr::static_vector<life_fragment, 9> m_life_fragments;
	std::array<fragment, 6> m_shattered_life_fragments;
	int m_collected_fragments;
	ticks m_time_since_last_life_fragments;
	int m_lives_left;
	ticks m_time_since_extend;
	ticks m_time_since_hit;
	ticks m_time_since_game_over;
	ticks m_accumulated_lives_hover_time;
	ticks m_accumulated_timer_hover_time;
	bool m_tock;

	glm::vec2 timer_text_size(const std::string& text, float scale) const;

	void play_tick_sound_if_needed();
	void update_timers();
	void update_life_fragments();
	void check_if_player_is_hovering_over_timer();
	void check_if_player_is_hovering_over_lives();
	void check_if_player_was_hit();
	void set_up_shattered_life_fragments();
	void check_if_player_collected_life_fragments();
	void set_screen_shake() const;

	void add_timer_to_renderer() const;
	void add_lives_to_renderer() const;
	void add_appearing_life_to_renderer(tr::rgb8 color, u8 base_opacity) const;
	void add_shattering_life_to_renderer(tr::rgb8 color, u8 base_opacity) const;
};

/////////////////////////////////////////////////////////////// ACTIVE GAME ///////////////////////////////////////////////////////////////

// Game that is actively being played.
class active_game : public game {
  public:
	active_game(const ::gamemode& gamemode, u64 seed = engine::rng.generate<u64>());

	void update() override;

	replay replay;
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
};