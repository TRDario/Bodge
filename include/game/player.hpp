#pragma once
#include "../gamemode.hpp"
#include "trail.hpp"

class ball;

// Player object.
class player {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a player.
	player(const player_settings& settings, ticks pb);

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Gets whether the game is over.
	bool game_over() const;
	// Gets the amount of time that passed since game over.
	ticks time_since_game_over() const;
	// Checks whether a player and any balls are colliding.
	friend bool colliding(const player& player, const tr::static_vector<ball, 255>& balls);

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Hits the player.
	void hit();
	// Updates the player state without updating the position.
	void update();
	// Updates the player state.
	void update(glm::vec2 target);

	////////////////////////////////////////////////////////////// GRAPHICS ///////////////////////////////////////////////////////////////

	// Adds the player graphic to the renderer drawing list.
	void add_to_renderer() const;

  private:
	// Death fragment.
	struct death_fragment {
		// The position of the fragment.
		glm::vec2 pos;
		// The velocity of the fragment.
		glm::vec2 vel;
		// The rotation of the fragment.
		tr::angle rot;
		// The angular velocity of the fragment.
		tr::angle rotvel;

		// Updates the fragment.
		void update();
	};

	// The player hitbox.
	tr::circle m_hitbox;
	// Control points of the player's trail.
	trail m_trail;
	// The death fragments.
	std::array<death_fragment, 6> m_fragments;
	// The number of lives left.
	int m_lives;
	// The player's movement inertia.
	float m_inertia;
	// Internal timer.
	ticks m_timer;
	// Internal timer.
	ticks m_game_over_timer;
	// The remaining invulnerability in ticks (0 if not invulnerable).
	ticks m_iframes;
	// The accumulated time spent hovering over the lives UI.
	ticks m_lives_hover_time;
	// The accumulated time spent hovering over the timer text.
	ticks m_timer_hover_time;
	// An atlas holding the timer graphics.
	tr::dyn_atlas<char> m_atlas;
	// Previous personal best.
	ticks m_pb;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Gets the size of timer text.
	glm::vec2 timer_text_size(const std::string& text, float scale) const;
	// Sets up the fragments for the death animation.
	void set_up_death_fragments();

	// Adds the player fill to the renderer.
	void add_fill_to_renderer(std::uint8_t opacity, tr::angle rotation, float size) const;
	// Adds the player outline to the renderer.
	void add_outline_to_renderer(tr::rgb8 tint, std::uint8_t opacity, tr::angle rotation, float size) const;
	// Adds the player trail to the renderer.
	void add_trail_to_renderer(tr::rgb8 tint, std::uint8_t opacity, tr::angle rotation, float size) const;
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