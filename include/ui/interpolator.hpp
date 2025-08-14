#pragma once
#include "../global.hpp"

// Interpolation modes.
enum class interp {
	// Uses linear interpolation from start to end.
	LERP,
	// Uses cubic interpolation from start to end.
	CUBIC,
	// Cycles in a sinusoidal pattern from start to end.
	CYCLE
};

template <class T> struct interpolator {
  public:
	// Constructs an interpolator with a value.
	constexpr interpolator(T value);
	// Constructs an interpolator with an ongoing interpolation.
	constexpr interpolator(interp mode, T start, T end, ticks time);

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update();
	// Gets whether the interpolation is done.
	bool done() const;
	// Gets the current value of the interpolator.
	operator T() const;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an easing interpolation starting from the current value.
	void change(interp mode, T end, ticks time);
	// Sets the interpolator.
	interpolator& operator=(T r);

  private:
	// The current interpolation mode.
	interp m_mode;
	// The value at the start of the interpolation.
	T m_start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	T m_end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	std::uint16_t m_len;
	// The current position within the interpolation.
	std::uint16_t m_pos;
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <class T>
constexpr interpolator<T>::interpolator(T value)
	: m_mode{interp::CUBIC}, m_end{value}, m_len{0}, m_pos{0}
{
}

template <class T>
constexpr interpolator<T>::interpolator(interp mode, T start, T end, ticks time)
	: m_mode{mode}, m_start{start}, m_end{end}, m_len{static_cast<std::uint16_t>(time)}, m_pos{0}
{
}

template <class T> void interpolator<T>::update()
{
	if (m_len != 0 && ++m_pos == m_len) {
		switch (m_mode) {
		case interp::LERP:
		case interp::CUBIC:
			m_len = 0;
			break;
		case interp::CYCLE:
			m_pos = 0;
		}
	}
}

template <class T> bool interpolator<T>::done() const
{
	return m_len == 0;
}

template <class T> interpolator<T>::operator T() const
{
	if (m_len == 0) {
		return m_end;
	}
	else {
		float ratio;
		switch (m_mode) {
		case interp::LERP:
			ratio = static_cast<float>(m_pos) / m_len;
			break;
		case interp::CUBIC:
			ratio = static_cast<float>(m_pos) / m_len;
			ratio = ratio < 0.5f ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
			break;
		case interp::CYCLE:
			ratio = (tr::turns(static_cast<float>(m_pos) / m_len).cos() - 1) / -2.0f;
			break;
		}
		return m_start + m_end * ratio - m_start * ratio;
	}
}

template <class T> void interpolator<T>::change(interp mode, T end, ticks time)
{
	*this = interpolator{mode, *this, end, static_cast<std::uint16_t>(time)};
}

template <class T> interpolator<T>& interpolator<T>::operator=(T r)
{
	return *this = interpolator{r};
}