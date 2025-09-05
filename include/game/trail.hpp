#pragma once
#include "../global.hpp"

// Player/ball trail circular buffer.
class trail {
  public:
	trail(const glm::vec2& value);

	static constexpr usize size();
	const glm::vec2& operator[](usize i) const;

	// Pushes a new control point to the front of the trail.
	void push(const glm::vec2& value);

  private:
	std::array<glm::vec2, 0.2_s> m_buffer;
	u32 m_head; // The current position of the head of the circular buffer.
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

constexpr usize trail::size()
{
	return 0.2_s;
}