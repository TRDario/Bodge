#pragma once
#include "widget.hpp"

//////////////////////////////////////////////////////////////// DECREMENT ////////////////////////////////////////////////////////////////

template <class T, T Limit> bool dec::compare(T v)
{
	return v > Limit;
}

template <class T, T Limit, T SmallChange, T MediumChange, T LargeChange> void dec::execute(T& v)
{
	if constexpr (std::unsigned_integral<T>) {
		v = T(std::max(i64(v) - engine::keymods_choose<i64>(SmallChange, MediumChange, LargeChange), i64(Limit)));
	}
	else {
		v = std::max(v - engine::keymods_choose<T>(SmallChange, MediumChange, LargeChange), Limit);
	}
}

//////////////////////////////////////////////////////////////// INCREMENT ////////////////////////////////////////////////////////////////

template <class T, T Limit> bool inc::compare(T v)
{
	return v < Limit;
}

template <class T, T Limit, T SmallChange, T MediumChange, T LargeChange> void inc::execute(T& v)
{
	if constexpr (std::unsigned_integral<T>) {
		v = T(std::min(i64(v) + engine::keymods_choose<i64>(SmallChange, MediumChange, LargeChange), i64(Limit)));
	}
	else {
		v = std::min(v + engine::keymods_choose<T>(SmallChange, MediumChange, LargeChange), Limit);
	}
}

////////////////////////////////////////////////////////// NUMERIC ARROW WIDGET ///////////////////////////////////////////////////////////

template <class T, tr::one_of<dec, inc> Operation, T Limit, T SmallChange, T MediumChange, T LargeChange>
numeric_arrow_widget<T, Operation, Limit, SmallChange, MediumChange, LargeChange>::numeric_arrow_widget(
	tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, status_callback base_status_cb, T& ref)
	: arrow_widget{pos,
				   alignment,
				   unhide_time,
				   std::same_as<inc, Operation>,
				   [base = std::move(base_status_cb), &ref] { return base() && Operation::template compare<T, Limit>(ref); },
				   [&ref] { Operation::template execute<T, Limit, SmallChange, MediumChange, LargeChange>(ref); }}
{
}