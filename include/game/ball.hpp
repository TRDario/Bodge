#pragma once
#include "trail.hpp"

// Ball object.
class ball {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a ball.
	ball(const tr::fcircle& hitbox, const glm::vec2& velocity) noexcept;
	// Randomly generates a ball.
	ball(tr::xorshiftr_128p& rng, float size, float velocity) noexcept;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Gets whether the ball is tangible.
	bool tangible() const noexcept;
	// Gets the hitbox of the player.
	const tr::fcircle& hitbox() const noexcept;
	// Checks whether two balls are colliding.
	friend bool colliding(const ball& a, const ball& b) noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Handles the physics interaction between two balls.
	friend void handle_collision(ball& a, ball& b) noexcept;
	// Updates the ball state.
	void update() noexcept;

	/////////////////////////////////////////////////////////////// GRAPHICS //////////////////////////////////////////////////////////////

	// Adds the ball graphic to the renderer drawing list.
	void add_to_renderer() const;

  private:
	// The ball hitbox.
	tr::fcircle _hitbox;
	// Control points of the ball's trail.
	trail _trail;
	// The ball's movement velocity.
	glm::vec2 _velocity;
	// The ball's age in ticks.
	ticks _age;
	// The time since the last collision event this ball was part of in ticks.
	ticks _last_collision;
};