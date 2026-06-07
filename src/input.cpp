#include "../include/input.hpp"
#include "../include/renderer.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////////// INPUT //////////////////////////////////////////////////////////////////

bool input::held(tr::sys::keymod modifiers) const
{
	return bool(m_held_keymods & modifiers);
}

bool input::held(tr::sys::mouse_button buttons) const
{
	return bool(m_held_buttons & buttons);
}

//

struct input::event_handler {
	input& input;

	void operator()(tr::sys::window_gain_focus_event) const
	{
		tr::sys::set_mouse_relative_mode(true);
	}

	void operator()(tr::sys::window_lose_focus_event) const
	{
		tr::sys::set_mouse_relative_mode(false);
	}

	void operator()(tr::one_of<tr::sys::key_down_event, tr::sys::key_up_event> auto event) const
	{
		input.m_held_keymods = event.mods;
	}

	void operator()(tr::sys::mouse_motion_event event)
	{
		if (tr::sys::window_has_focus()) {
			const float scale{renderer::instance().scale() * tr::sys::window_pixel_density()};
			const float multiplier{active_settings::instance()->mouse_sensitivity / 100.0f / scale};
			const glm::vec2 delta{event.delta * multiplier};
			input.mouse_pos = glm::clamp(input.mouse_pos + delta, 0.0f, 1000.0f);
		}
	}

	void operator()(tr::sys::mouse_down_event event) const
	{
		input.m_held_buttons |= event.button;
	}

	void operator()(tr::sys::mouse_up_event event) const
	{
		input.m_held_buttons &= ~event.button;
	}

	void operator()(auto) const {}
};

void input::handle_event(const tr::sys::event& event)
{
	event.visit(event_handler{*this});
}