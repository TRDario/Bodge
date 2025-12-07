#include "../../include/ui/tweening.hpp"

//////////////////////////////////////////////////////////// TWEENED POSITION /////////////////////////////////////////////////////////////

void tweened_position::tick()
{
	if (m_duration != 0 && ++m_elapsed == m_duration) {
		m_duration = 0;
	}
}

bool tweened_position::done() const
{
	return m_duration == 0;
}

tweened_position::operator glm::vec2() const
{
	if (m_duration == 0) {
		return m_end;
	}
	else {
		// Cubic interpolation.
		float ratio{float(m_elapsed) / m_duration};
		ratio = ratio < 0.5f ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
		return m_start + m_end * ratio - m_start * ratio;
	}
}

void tweened_position::move(glm::vec2 end, ticks time)
{
	*this = tweened_position{*this, end, time};
}

void tweened_position::move_x(float end, ticks time)
{
	*this = tweened_position{*this, {end, m_end.y}, time};
}

void tweened_position::move_y(float end, ticks time)
{
	*this = tweened_position{*this, {m_end.x, end}, time};
}

tweened_position& tweened_position::operator=(glm::vec2 r)
{
	return *this = tweened_position{r};
}

////////////////////////////////////////////////////////////// TWEENED COLOR //////////////////////////////////////////////////////////////

void tweened_color::tick()
{
	if (m_duration != 0 && ++m_elapsed == m_duration) {
		m_duration = 0;
	}
}

bool tweened_color::done() const
{
	return m_duration == 0;
}

bool tweened_color::cycling() const
{
	return m_cycling;
}

tweened_color::operator tr::rgba8() const
{
	if (m_duration == 0) {
		return m_end;
	}
	else {
		float ratio{float(m_elapsed) / m_duration};
		if (m_cycling) {
			ratio = (tr::turns(ratio).cos() - 1) / -2.0f;
		}
		return m_start + m_end * ratio - m_start * ratio;
	}
}

void tweened_color::change(tr::rgba8 end, ticks time, cycle cycle)
{
	*this = tweened_color{*this, end, time, cycle};
}

tweened_color& tweened_color::operator=(tr::rgba8 r)
{
	return *this = tweened_color{r};
}

///////////////////////////////////////////////////////////// TWEENED OPACITY /////////////////////////////////////////////////////////////

void tweened_opacity::tick()
{
	if (m_duration != 0 && ++m_elapsed == m_duration) {
		m_duration = 0;
	}
}

bool tweened_opacity::done() const
{
	return m_duration == 0;
}

tweened_opacity::operator float() const
{
	if (m_duration == 0) {
		return m_end;
	}
	else {
		// Cubic interpolation.
		float ratio{float(m_elapsed) / m_duration};
		ratio = ratio < 0.5f ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
		return m_start + m_end * ratio - m_start * ratio;
	}
}

void tweened_opacity::change(float end, ticks time)
{
	*this = tweened_opacity{*this, end, time};
}

tweened_opacity& tweened_opacity::operator=(float r)
{
	return *this = tweened_opacity{r};
}