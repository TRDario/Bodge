#include "../../include/ui/interpolated.hpp"

interpolated_float::interpolated_float(float value) noexcept
	: _end{value}, _len{0}
{
}

interpolated_float::interpolated_float(float start, float end, std::uint16_t time) noexcept
	: _start{start}, _end{end}, _len{time}, _pos{0}
{
}

void interpolated_float::update() noexcept
{
	if (_len != 0 && ++_pos == _len) {
		_len = 0;
	}
}

interpolated_float::operator float() const noexcept
{
	if (_len == 0) {
		return _end;
	}
	else {
		float ratio{static_cast<float>(_pos) / _len};
		ratio = ratio < 0.5 ? 4 * std::pow(ratio, 3) : 1 - std::pow(-2 * ratio + 2, 3) / 2;
		return _start + (_end - _start) * ratio;
	}
}

void interpolated_float::change(float end, std::uint16_t time) noexcept
{
	*this = interpolated_float{*this, end, time};
}

interpolated_float& interpolated_float::operator=(float r) noexcept
{
	return *this = interpolated_float{r};
}

//

interpolated_vec2::interpolated_vec2(glm::vec2 value) noexcept
	: _end{value}, _len{0}
{
}

interpolated_vec2::interpolated_vec2(glm::vec2 start, glm::vec2 end, std::uint16_t time) noexcept
	: _start{start}, _end{end}, _len{time}, _pos{0}
{
}

void interpolated_vec2::update() noexcept
{
	if (_len != 0 && ++_pos == _len) {
		_len = 0;
	}
}

interpolated_vec2::operator glm::vec2() const noexcept
{
	if (_len == 0) {
		return _end;
	}
	else {
		float ratio{static_cast<float>(_pos) / _len};
		ratio = ratio < 0.5 ? 4 * std::pow(ratio, 3) : 1 - std::pow(-2 * ratio + 2, 3) / 2;
		return _start + (_end - _start) * ratio;
	}
}

void interpolated_vec2::change(glm::vec2 end, std::uint16_t time) noexcept
{
	*this = interpolated_vec2{*this, end, time};
}

interpolated_vec2& interpolated_vec2::operator=(glm::vec2 r) noexcept
{
	return *this = interpolated_vec2{r};
}

//

interpolated_rgba8::interpolated_rgba8(tr::rgba8 value) noexcept
	: _end{value}, _len{0}
{
}

interpolated_rgba8::interpolated_rgba8(tr::rgba8 start, tr::rgba8 end, std::uint16_t time) noexcept
	: _start{start}, _end{end}, _len{time}, _pos{0}
{
}

void interpolated_rgba8::update() noexcept
{
	if (_len != 0 && ++_pos == _len) {
		_len = 0;
	}
}

interpolated_rgba8::operator tr::rgba8() const noexcept
{
	if (_len == 0) {
		return _end;
	}
	else {
		float ratio{static_cast<float>(_pos) / _len};
		return {static_cast<std::uint8_t>(_start.r + (_end.r - _start.r) * ratio),
				static_cast<std::uint8_t>(_start.g + (_end.g - _start.g) * ratio),
				static_cast<std::uint8_t>(_start.b + (_end.b - _start.b) * ratio),
				static_cast<std::uint8_t>(_start.a + (_end.a - _start.a) * ratio)};
	}
}

void interpolated_rgba8::change(tr::rgba8 end, std::uint16_t time) noexcept
{
	*this = interpolated_rgba8{*this, end, time};
}

interpolated_rgba8& interpolated_rgba8::operator=(tr::rgba8 r) noexcept
{
	return *this = interpolated_rgba8{r};
}