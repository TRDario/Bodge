#include "../../include/game/trail.hpp"

trail::trail(const glm::vec2& value)
	: m_head{size() - 1}
{
	m_buffer.fill(value);
}

//

const glm::vec2& trail::operator[](usize i) const
{
	assert(i < size());

	return m_buffer[((m_head - i) + size()) % size()];
}

//

void trail::push(const glm::vec2& value)
{
	m_head = (m_head + 1) % size();
	m_buffer[m_head] = value;
}