#include "../../include/game/trail.hpp"

trail::trail(const vec2& value) noexcept
	: _head{SIZE - 1}
{
	_buffer.fill(value);
}

const vec2& trail::operator[](size_t i) const noexcept
{
	assert(i < SIZE);

	return _buffer[((_head - i) + SIZE) % SIZE];
}

void trail::push(const vec2& value) noexcept
{
	_head = (_head + 1) % SIZE;
	_buffer[_head] = value;
}