#pragma once
#include "settings.hpp"

namespace engine {
	/////////////////////////////////////////////////////////////// LIFETIME //////////////////////////////////////////////////////////////

	// Initializes the engine.
	void initialize_system();
	// Sets the game state to the main menu state.
	void set_main_menu_state();
	// Applies new settings to the engine.
	void apply_settings(const ::settings& old);
	// Shuts the engine down.
	void shut_down_system();

	//////////////////////////////////////////////////////////////// INPUT ////////////////////////////////////////////////////////////////

	// Gets whether the engine is active.
	bool active();
	// Handles any events.
	void handle_events();
	// Redraws the screen if needed.
	void redraw_if_needed();
	// Gets the held keyboard modifiers.
	tr::keymod held_keymods();
	// Chooses one out of 3 options based on the held keymods.
	template <class T> T keymods_choose(T min, T mid, T max);
	// Gets the normalized mouse position.
	glm::vec2 mouse_pos();
	// Sets the normalized mouse position.
	void set_mouse_pos(glm::vec2 pos);
	// Gets the held mouse buttons.
	tr::mouse_button held_buttons();
}; // namespace engine

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <class T> T engine::keymods_choose(T min, T mid, T max)
{
	if (held_keymods() & tr::keymod::CTRL) {
		return max;
	}
	else if (held_keymods() & tr::keymod::SHIFT) {
		return mid;
	}
	else {
		return min;
	}
}