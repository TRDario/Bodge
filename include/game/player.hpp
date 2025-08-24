#pragma once
#include "../gamemode.hpp"
#include "trail.hpp"

class ball;

// Player object.
class player {
  public:
	player(const player_settings& settings, ticks prev_pb);

	bool game_over() const;
	ticks time_since_game_over() const;
	bool colliding_with_any_of(const tr::static_vector<ball, 255>& balls);

	void hit();
	void update();
	void update(glm::vec2 target);

	void add_to_renderer() const;

  private:
	// Visual death fragment information.
	struct death_fragment {
		glm::vec2 pos;
		glm::vec2 vel;
		tr::angle rot;
		tr::angle rotvel;

		void update();
	};

	tr::circle m_hitbox;
	trail m_trail;
	std::array<death_fragment, 6> m_fragments;
	int m_lives_left;
	float m_inertia;
	ticks m_timer;
	ticks m_game_over_timer;
	ticks m_iframes_left;
	ticks m_lives_hover_time; // The accumulated time spent hovering over the lives UI.
	ticks m_timer_hover_time; // The accumulated time spent hovering over the timer text.
	tr::gfx::dyn_atlas<char> m_atlas;
	ticks m_prev_pb;

	glm::vec2 timer_text_size(const std::string& text, float scale) const;
	void set_up_death_fragments();

	void add_fill_to_renderer(u8 opacity, tr::angle rotation, float size) const;
	void add_outline_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const;
	void add_trail_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const;
	void add_lives_to_renderer() const;
	void add_timer_to_renderer() const;
	void set_screen_shake() const;
	void add_death_wave_to_renderer() const;
	void add_death_fragments_to_renderer() const;
};