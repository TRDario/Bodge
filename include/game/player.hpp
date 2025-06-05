#pragma once
#include "../gamemode.hpp"
#include "trail.hpp"

class ball;

// Player object.
class player {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a player.
	player(const player_settings& settings, ticks pb) noexcept;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Gets whether the game is over.
	bool game_over() const noexcept;
	// Gets the amount of time that passed since game over.
	ticks time_since_game_over() const noexcept;
	// Checks whether a player and any balls are colliding.
	friend bool colliding(const player& player, const static_vector<ball, 255>& balls) noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Hits the player.
	void hit() noexcept;
	// Updates the player state without updating the position.
	void update() noexcept;
	// Updates the player state.
	void update(vec2 target) noexcept;

	////////////////////////////////////////////////////////////// GRAPHICS ///////////////////////////////////////////////////////////////

	// Adds the player graphic to the renderer drawing list.
	void add_to_renderer() const;

  private:
	// Death fragment.
	struct death_fragment {
		// The position of the fragment.
		vec2 pos;
		// The velocity of the fragment.
		vec2 vel;
		// The rotation of the fragment.
		fangle rot;
		// The angular velocity of the fragment.
		fangle rotvel;

		// Updates the fragment.
		void update() noexcept;
	};

	// The player hitbox.
	tr::fcircle _hitbox;
	// Control points of the player's trail.
	trail _trail;
	// The death fragments.
	array<death_fragment, 6> _fragments;
	// The number of lives left.
	int _lives;
	// The player's movement inertia.
	float _inertia;
	// Internal timer.
	ticks _timer;
	// Internal timer.
	ticks _game_over_timer;
	// The remaining invulnerability in ticks (0 if not invulnerable).
	ticks _iframes;
	// The accumulated time spent hovering over the lives UI.
	ticks _lives_hover_time;
	// The accumulated time spent hovering over the timer text.
	ticks _timer_hover_time;
	// An atlas holding the timer graphics.
	tr::static_atlas _atlas;
	// Previous personal best.
	ticks _pb;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Gets the size of timer text.
	vec2 timer_text_size(const string& text, float scale) const noexcept;
	// Sets up the fragments for the death animation.
	void set_up_death_fragments() noexcept;

	// Adds the player fill to the renderer.
	void add_fill_to_renderer(u8 opacity, fangle rotation, float size) const;
	// Adds the player outline to the renderer.
	void add_outline_to_renderer(tr::rgb8 tint, u8 opacity, fangle rotation, float size) const;
	// Adds the player trail to the renderer.
	void add_trail_to_renderer(tr::rgb8 tint, u8 opacity, fangle rotation, float size) const;
	// Adds the lives UI to the renderer.
	void add_lives_to_renderer() const;
	// Adds the timer text to the renderer.
	void add_timer_to_renderer() const;
	// Sets the screen shake.
	void set_screen_shake() const;
	// Adds the death wave to the renderer.
	void add_death_wave_to_renderer() const;
	// Adds the death fragments to the renderer.
	void add_death_fragments_to_renderer() const;
};