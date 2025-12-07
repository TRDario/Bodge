#pragma once
#include "../global.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Size of player and ball trails.
inline constexpr usize TRAIL_SIZE{0.2_s};

////////////////////////////////////////////////////////////////// TRAIL //////////////////////////////////////////////////////////////////

// Player/ball trail circular buffer.
class trail {
  public:
	// Creates a trail with an initial value.
	trail(const glm::vec2& value);

	// Gets the point at the given index.
	const glm::vec2& operator[](usize i) const;

	// Pushes a new control point to the front of the trail.
	void push(const glm::vec2& value);

  private:
	// Array used as the backing for the circular buffer.
	std::array<glm::vec2, TRAIL_SIZE> m_buffer;
	// The current position of the head of the circular buffer.
	u32 m_head;
};