#pragma once
#include "../gamemode.hpp"
#include "trail.hpp"

class ball;
class life_fragment;

// Player object.
class player {
  public:
	player(const player_settings& settings);

	bool invincible() const;
	const tr::circle& hitbox() const;

	void hit();
	void kill();
	void update(glm::vec2 target);
	void update_fragments();

	void add_to_renderer_alive(ticks time_since_start) const;
	void add_to_renderer_dead(ticks time_since_game_over) const;

  private:
	tr::circle m_hitbox;
	trail m_trail;
	std::array<fragment, 6> m_fragments;
	float m_inertia;
	ticks m_iframes_left;

	void add_fill_to_renderer(u8 opacity, tr::angle rotation, float size) const;
	void add_outline_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const;
	void add_trail_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const;
	void add_death_wave_to_renderer(ticks time_since_game_over) const;
	void add_death_fragments_to_renderer(ticks time_since_game_over) const;
};