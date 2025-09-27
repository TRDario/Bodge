#pragma once
#include "settings.hpp"

namespace engine {
	void initialize_system();
	void set_main_menu_state();
	bool restart_required(const ::settings& old);
	void apply_settings(const ::settings& old);
	void shut_down_system();

	bool active();
	void handle_events();
	void redraw_if_needed();

	tr::sys::keymod held_keymods();
	template <class T> T keymods_choose(T min, T mid, T max); // Chooses one out of 3 options based on the held keymods.
	glm::vec2 mouse_pos();
	void set_mouse_pos(glm::vec2 pos);
	tr::sys::mouse_button held_buttons();
}; // namespace engine

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <class T> T engine::keymods_choose(T min, T mid, T max)
{
	if (held_keymods() & tr::sys::keymod::CTRL) {
		return max;
	}
	else if (held_keymods() & tr::sys::keymod::SHIFT) {
		return mid;
	}
	else {
		return min;
	}
}