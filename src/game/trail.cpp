#include "../../include/game/trail.hpp"

trail::trail(const glm::vec2& value)
	: m_head{SIZE - 1}
{
	m_buffer.fill(value);
}

//

const glm::vec2& trail::operator[](usize i) const
{
	assert(i < SIZE);

	return m_buffer[((m_head - i) + SIZE) % SIZE];
}

//

void trail::push(const glm::vec2& value)
{
	m_head = (m_head + 1) % SIZE;
	m_buffer[m_head] = value;
}