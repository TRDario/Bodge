///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides various timer classes.                                                                                                       //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "global.hpp"

////////////////////////////////////////////////////////////////// TIMER //////////////////////////////////////////////////////////////////

// Infinitely-incrementing timer with a special inactive state.
class startable_timer {
  public:
	// Gets whether the timer is active.
	inline bool active() const;
	// Gets the elapsed time since the timer started.
	inline ticks elapsed() const;

	// Starts the timer.
	inline void start();
	// Updates the timer.
	inline void tick();

  private:
	// Elapsed ticks or max() to denote a stopped timer.
	ticks m_time{std::numeric_limits<ticks>::max()};
};

// Fixed-duration timer that counts down.
template <ticks START> class decrementing_timer {
  public:
	// Gets whether the timer is active.
	bool active() const;
	// Gets the ratio (elapsed time) / (total time).
	float elapsed_ratio() const;

	// Starts the timer.
	void start();
	// Updates the timer.
	void tick();

  private:
	// Number of ticks left to decrement.
	ticks m_left{0};
};

// Timer that can count up or down up to a maximum.
template <ticks MAX> class accumulating_timer {
  public:
	// Gets the maximum number of ticks that can be acumulated.
	static constexpr ticks max();

	// Gets the accumulated number of ticks.
	ticks accumulated() const;

	// Increments the timer.
	void increment();
	// Decrements the timer.
	void decrement();

  private:
	// Accumulated number of ticks.
	ticks m_accumulated{0};
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

bool startable_timer::active() const
{
	return m_time != std::numeric_limits<ticks>::max();
}

ticks startable_timer::elapsed() const
{
	return m_time;
}

void startable_timer::start()
{
	m_time = 0;
}

void startable_timer::tick()
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

template <ticks START> void decrementing_timer<START>::tick()
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