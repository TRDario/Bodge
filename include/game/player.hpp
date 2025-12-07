#pragma once
#include "../gamemode.hpp"
#include "../timer.hpp"
#include "trail.hpp"

/////////////////////////////////////////////////////////// FORWARD DECLARATIONS //////////////////////////////////////////////////////////

class ball;
class life_fragment;

////////////////////////////////////////////////////////////////// PLAYER /////////////////////////////////////////////////////////////////

// Player object.
class player {
  public:
	// Creates a player.
	player(const player_settings& settings);

	// Gets whether the player is currently invincible.
	bool invincible() const;
	// Gets the player's hitbox.
	const tr::circle& hitbox() const;

	// Hits the player.
	void hit();
	// Kills the player.
	void kill();
	// Updates the player's state.
	void tick(glm::vec2 target);
	// Updates the state of the player's death fragments.
	void update_fragments();

	// Adds the living player to the renderer.
	void add_to_renderer_alive(ticks time_since_start, const decrementing_timer<0.1_s>& style_cooldown_timer) const;
	// Adds the dead player to the renderer.
	void add_to_renderer_dead(ticks time_since_game_over) const;

  private:
	// The player's hitbox.
	tr::circle m_hitbox;
	// The player's trail.
	trail m_trail;
	// List of the player's fragments when they die.
	std::array<fragment, 6> m_fragments;
	// The player's movement inertia factor.
	float m_inertia;
	// Timer controlling the player's invincibility.
	decrementing_timer<2_s> m_invincibility_timer;

	// Adds the player visual's fill to the renderer.
	void add_fill_to_renderer(u8 opacity, tr::angle rotation, float size) const;
	// Adds the player visual's outline to the renderer.
	void add_outline_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const;
	// Adds the player's trail to the renderer.
	void add_trail_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const;
	// Adds the wave emitted after getting style points to the renderer.
	void add_style_wave_to_renderer(tr::rgb8 tint, const decrementing_timer<0.1_s>& timer) const;
	// Adds the player's death wave to the renderer.
	void add_death_wave_to_renderer(ticks time_since_game_over) const;
	// Adds the player's death fragments to the renderer.
	void add_death_fragments_to_renderer(ticks time_since_game_over) const;
};