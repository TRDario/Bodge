#pragma once
#include "global.hpp"

// Infinitely-incrementing timer with a special inactive state.
class startable_timer {
  public:
	inline bool active() const;
	inline ticks elapsed() const;

	inline void start();
	inline void update();

  private:
	ticks m_time{std::numeric_limits<ticks>::max()};
};

// Fixed-duration timer that counts down.
template <ticks START> class decrementing_timer {
  public:
	bool active() const;
	float elapsed_ratio() const;

	void start();
	void update();

  private:
	ticks m_left{0};
};

// Timer that can count up or down up to a maximum.
template <ticks MAX> class accumulating_timer {
  public:
	static constexpr ticks max();

	ticks accumulated() const;

	void increment();
	void decrement();

  private:
	ticks m_accumulated{0};
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

bool startable_timer::active() const
{
	return m_time != std::numeric_limits<ticks>::max();
}

ticks startable_timer::elapsed() const
{
	TR_ASSERT(active(), "Tried to get value of inactive timer.");

	return m_time;
}

void startable_timer::start()
{
	m_time = 0;
}

void startable_timer::update()
{
	if (active()) {
		++m_time;
	}
}

//

template <ticks START> bool decrementing_timer<START>::active() const
{
	return m_left != 0;
}

template <ticks START> float decrementing_timer<START>::elapsed_ratio() const
{
	return 1 - m_left / float(START);
}

template <ticks START> void decrementing_timer<START>::start()
{
	m_left = START;
}

template <ticks START> void decrementing_timer<START>::update()
{
	if (active()) {
		--m_left;
	}
}

//

template <ticks MAX> constexpr ticks accumulating_timer<MAX>::max()
{
	return MAX;
}

template <ticks MAX> ticks accumulating_timer<MAX>::accumulated() const
{
	return m_accumulated;
}

template <ticks MAX> void accumulating_timer<MAX>::increment()
{
	if (m_accumulated < MAX) {
		++m_accumulated;
	}
}

template <ticks MAX> void accumulating_timer<MAX>::decrement()
{
	if (m_accumulated > 0) {
		--m_accumulated;
	}
}