#pragma once
#include "trail.hpp"

// Ball object.
class ball {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a ball.
	ball(const tr::circle& hitbox, const glm::vec2& velocity);
	// Randomly generates a ball.
	ball(tr::xorshiftr_128p& rng, float size, float velocity);

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Gets whether the ball is tangible.
	bool tangible() const;
	// Gets the hitbox of the player.
	const tr::circle& hitbox() const;
	// Checks whether two balls are colliding.
	friend bool colliding(const ball& a, const ball& b);

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Handles the physics interaction between two balls.
	friend void handle_collision(ball& a, ball& b);
	// Updates the ball state.
	void update();

	/////////////////////////////////////////////////////////////// GRAPHICS //////////////////////////////////////////////////////////////

	// Adds the ball graphic to the renderer drawing list.
	void add_to_renderer() const;

  private:
	// The ball hitbox.
	tr::circle m_hitbox;
	// Control points of the ball's trail.
	trail m_trail;
	// The ball's movement velocity.
	glm::vec2 m_velocity;
	// The ball's age in ticks.
	ticks m_age;
	// The time since the last collision event this ball was part of in ticks.
	ticks m_last_collision;
};