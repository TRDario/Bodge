#pragma once
#include "trail.hpp"

/////////////////////////////////////////////////////////////////// BALL //////////////////////////////////////////////////////////////////

// Ball object.
class ball {
  public:
	// Creates a ball.
	ball(const tr::circle& hitbox, const glm::vec2& velocity);
	// Randomly generates a ball.
	ball(tr::xorshiftr_128p& rng, float size, float velocity);

	// Gets whether the ball is tangible.
	bool tangible() const;
	// Gets the hitbox of the ball.
	const tr::circle& hitbox() const;
	// Gets the velocity of the ball.
	const glm::vec2& velocity() const;

	// Updates the ball's state.
	void tick();

	// Adds the ball to the renderer.
	void add_to_renderer() const;

  private:
	// The ball's hitbox.
	tr::circle m_hitbox;
	// The ball's trail.
	trail m_trail;
	// The ball's current velocity.
	glm::vec2 m_velocity;
	// Time elapsed since the ball was spawned.
	ticks m_age;
	// Time elapsed since the ball last hit something.
	ticks m_time_since_last_collision;

	friend void handle_collision(ball& a, ball& b);
};

// Gets whether two balls are colliding.
bool colliding(const ball& a, const ball& b);
// Handles the collision between two balls.
void handle_collision(ball& a, ball& b);