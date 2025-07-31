#include "../../include/ui/interpolated.hpp"

interpolated_float::interpolated_float(float value)
	: start{}, end{value}, len{0}, pos{0}
{
}

interpolated_float::interpolated_float(float start, float end, std::uint16_t time)
	: start{start}, end{end}, len{time}, pos{0}
{
}

void interpolated_float::update()
{
	if (len != 0 && ++pos == len) {
		len = 0;
	}
}

interpolated_float::operator float() const
{
	if (len == 0) {
		return end;
	}
	else {
		float ratio{static_cast<float>(pos) / len};
		ratio = ratio < 0.5f ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
		return start + (end - start) * ratio;
	}
}

void interpolated_float::change(float end, ticks time)
{
	*this = interpolated_float{*this, end, static_cast<std::uint16_t>(time)};
}

interpolated_float& interpolated_float::operator=(float r)
{
	return *this = interpolated_float{r};
}

//

interpolated_vec2::interpolated_vec2(glm::vec2 value)
	: start{}, end{value}, len{0}, pos{0}
{
}

interpolated_vec2::interpolated_vec2(glm::vec2 start, glm::vec2 end, std::uint16_t time)
	: start{start}, end{end}, len{time}, pos{0}
{
}

void interpolated_vec2::update()
{
	if (len != 0 && ++pos == len) {
		len = 0;
	}
}

interpolated_vec2::operator glm::vec2() const
{
	if (len == 0) {
		return end;
	}
	else {
		float ratio{static_cast<float>(pos) / len};
		ratio = ratio < 0.5f ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
		return start + (end - start) * ratio;
	}
}

void interpolated_vec2::change(glm::vec2 end, ticks time)
{
	*this = interpolated_vec2{*this, end, static_cast<std::uint16_t>(time)};
}

interpolated_vec2& interpolated_vec2::operator=(glm::vec2 r)
{
	return *this = interpolated_vec2{r};
}

//

interpolated_rgba8::interpolated_rgba8(tr::rgba8 value)
	: start{}, end{value}, len{0}, pos{0}
{
}

interpolated_rgba8::interpolated_rgba8(tr::rgba8 start, tr::rgba8 end, std::uint16_t time)
	: start{start}, end{end}, len{time}, pos{0}
{
}

void interpolated_rgba8::update()
{
	if (len != 0 && ++pos == len) {
		len = 0;
	}
}

interpolated_rgba8::operator tr::rgba8() const
{
	if (len == 0) {
		return end;
	}
	else {
		float ratio{static_cast<float>(pos) / len};
		return {
			static_cast<std::uint8_t>(start.r + (end.r - start.r) * ratio), static_cast<std::uint8_t>(start.g + (end.g - start.g) * ratio),
			static_cast<std::uint8_t>(start.b + (end.b - start.b) * ratio), static_cast<std::uint8_t>(start.a + (end.a - start.a) * ratio)};
	}
}

void interpolated_rgba8::change(tr::rgba8 end, ticks time)
{
	*this = interpolated_rgba8{*this, end, static_cast<std::uint16_t>(time)};
}

interpolated_rgba8& interpolated_rgba8::operator=(tr::rgba8 r)
{
	return *this = interpolated_rgba8{r};
}