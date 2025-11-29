#pragma once
#include "settings.hpp"

namespace engine {
	void initialize_system();
	bool restart_required(const ::settings& old);
	void apply_settings(const ::settings& old);

	template <class T> T keymods_choose(T min, T mid, T max); // Chooses one out of 3 options based on the held keymods.
}; // namespace engine

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <class T> T engine::keymods_choose(T min, T mid, T max)
{
	if (g_held_keymods & tr::sys::keymod::CTRL) {
		return max;
	}
	else if (g_held_keymods & tr::sys::keymod::SHIFT) {
		return mid;
	}
	else {
		return min;
	}
}