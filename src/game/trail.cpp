///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements game/trail.hpp.                                                                                                            //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/game/trail.hpp"

////////////////////////////////////////////////////////////////// TRAIL //////////////////////////////////////////////////////////////////

trail::trail(const glm::vec2& value)
	: m_head{TRAIL_SIZE - 1}
{
	m_buffer.fill(value);
}

//

const glm::vec2& trail::operator[](usize i) const
{
	assert(i < TRAIL_SIZE);

	return m_buffer[((m_head - i) + TRAIL_SIZE) % TRAIL_SIZE];
}

//

void trail::push(const glm::vec2& value)
{
	m_head = (m_head + 1) % TRAIL_SIZE;
	m_buffer[m_head] = value;
}