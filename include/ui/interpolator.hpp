#pragma once
#include "../global.hpp"

enum class tween {
	LERP,
	CUBIC,
	CYCLE
};

template <class T> struct tweener {
  public:
	constexpr tweener(T value);
	constexpr tweener(tween mode, T start, T end, ticks time);

	void update();
	bool done() const;
	bool cycling() const;
	operator T() const;

	void change(tween mode, T end, ticks time);
	tweener& operator=(T r);

  private:
	tween m_mode;
	T m_start;
	T m_end;
	std::uint16_t m_len; // 0 to mark no ongoing tweening.
	std::uint16_t m_pos;
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <class T>
constexpr tweener<T>::tweener(T value)
	: m_mode{tween::CUBIC}, m_end{value}, m_len{0}, m_pos{0}
{
}

template <class T>
constexpr tweener<T>::tweener(tween mode, T start, T end, ticks time)
	: m_mode{mode}, m_start{start}, m_end{end}, m_len{std::uint16_t(time)}, m_pos{0}
{
}

template <class T> void tweener<T>::update()
{
	if (m_len != 0 && ++m_pos == m_len) {
		switch (m_mode) {
		case tween::LERP:
		case tween::CUBIC:
			m_len = 0;
			break;
		case tween::CYCLE:
			m_pos = 0;
		}
	}
}

template <class T> bool tweener<T>::done() const
{
	return m_len == 0;
}

template <class T> bool tweener<T>::cycling() const
{
	return m_mode == tween::CYCLE;
}

template <class T> tweener<T>::operator T() const
{
	if (m_len == 0) {
		return m_end;
	}
	else {
		float ratio;
		switch (m_mode) {
		case tween::LERP:
			ratio = float(m_pos) / m_len;
			break;
		case tween::CUBIC:
			ratio = float(m_pos) / m_len;
			ratio = ratio < 0.5f ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
			break;
		case tween::CYCLE:
			ratio = (tr::turns(float(m_pos) / m_len).cos() - 1) / -2.0f;
			break;
		}
		return m_start + m_end * ratio - m_start * ratio;
	}
}

template <class T> void tweener<T>::change(tween mode, T end, ticks time)
{
	*this = tweener{mode, *this, end, std::uint16_t(time)};
}

template <class T> tweener<T>& tweener<T>::operator=(T r)
{
	return *this = tweener{r};
}