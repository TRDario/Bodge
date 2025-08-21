#pragma once
#include "trail.hpp"

// Ball object.
class ball {
  public:
	// Creates a ball.
	ball(const tr::circle& hitbox, const glm::vec2& velocity);
	// Randomly generates a ball.
	ball(tr::xorshiftr_128p& rng, float size, float velocity);

	bool tangible() const;
	const tr::circle& hitbox() const;
	friend bool colliding(const ball& a, const ball& b);

	friend void handle_collision(ball& a, ball& b);
	void update();

	void add_to_renderer() const;

  private:
	tr::circle m_hitbox;
	trail m_trail;
	glm::vec2 m_velocity;
	ticks m_time_since_spawned;
	ticks m_time_since_last_collision;
};