#include "../../include/game/trail.hpp"

trail::trail(const glm::vec2& value) noexcept
	: _head{SIZE - 1}
{
	_buffer.fill(value);
}

const glm::vec2& trail::operator[](std::size_t i) const noexcept
{
	assert(i < SIZE);

	return _buffer[((_head - i) + SIZE) % SIZE];
}

void trail::push(const glm::vec2& value) noexcept
{
	_head = (_head + 1) % SIZE;
	_buffer[_head] = value;
}