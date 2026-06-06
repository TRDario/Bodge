///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides an input manager singleton.                                                                                                  //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "global.hpp"

////////////////////////////////////////////////////////////////// INPUT //////////////////////////////////////////////////////////////////

// Input manager singleton.
class input {
  public:
	// Gets the input instance.
	static input& instance();

	// Position of the mouse in normalized screen coordinated.
	glm::vec2 mouse_pos{500, 500};
	// Gets whether one or multiple keyboard modifiers are held.
	bool held(tr::sys::keymod modifiers) const;
	// Chooses one of three values based on the currently held keymods.
	template <class T> T choose(T min, T mid, T max);
	// Gets whether one or multiple mouse buttons are held.
	bool held(tr::sys::mouse_button buttons) const;

	// Handles an event.
	void handle_event(const tr::sys::event& event);

  private:
	// The held keyboard modifiers.
	tr::sys::keymod m_held_keymods{tr::sys::keymod::NONE};
	// The held mouse buttons.
	tr::sys::mouse_button m_held_buttons{};

	// Constructs an input manager.
	input() = default;

	// Event handler visitor.
	struct event_handler;
};

////////////////////////////////////////////////////////////// IMPLEMENTATION /////////////////////////////////////////////////////////////

template <class T> T input::choose(T min, T mid, T max)
{
	return m_held_keymods & tr::sys::keymod::CTRL ? max : m_held_keymods & tr::sys::keymod::SHIFT ? mid : min;
}