#include "../../include/ui/ui_manager.hpp"
#include "../../include/system.hpp"

//

widget& ui_manager::operator[](tag tag)
{
	std::list<std::unique_ptr<widget>>::iterator it{
		std::ranges::find_if(m_objects, [=](std::unique_ptr<widget>& p) { return p->tag == tag; })};
	TR_ASSERT(it != m_objects.end(), "Tried to get widget with nonexistant tag \"{}\".", tag);
	return **it;
}

void ui_manager::clear()
{
	m_objects.clear();
	m_hovered = m_objects.end();
	m_input = m_objects.end();
}

//

void ui_manager::move_input_focus_forward()
{
	if (m_input == m_objects.end()) {
		for (std::list<std::unique_ptr<widget>>::iterator it = m_objects.begin(); it != m_objects.end(); ++it) {
			if ((*it)->writable()) {
				tr::system::enable_text_input_events();
				(*it)->on_gain_focus();
				m_input = it;
				return;
			}
		}
	}
	else {
		// Search to the end of the vector.
		for (std::list<std::unique_ptr<widget>>::iterator it = std::next(m_input); it != m_objects.end(); ++it) {
			if ((*it)->writable()) {
				(*m_input)->on_lose_focus();
				(*it)->on_gain_focus();
				m_input = it;
				return;
			}
		}
		// Nothing else found, just unselect.
		clear_input_focus();
	}
}

void ui_manager::move_input_focus_backward()
{
	if (m_input == m_objects.end()) {
		for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it) {
			if ((*it)->writable()) {
				(*it)->on_gain_focus();
				m_input = --(it.base());
				return;
			}
		}
	}
	else {
		// Search to the end of the vector.
		for (auto it = std::next(std::make_reverse_iterator(m_input)); it != m_objects.rend(); ++it) {
			if ((*it)->writable()) {
				(*it)->on_gain_focus();
				(*m_input)->on_lose_focus();
				m_input = --(it.base());
				return;
			}
		}
		// Nothing else found, just unselect.
		clear_input_focus();
	}
}

void ui_manager::clear_input_focus()
{
	if (m_input != m_objects.end()) {
		tr::system::disable_text_input_events();
		(*m_input)->on_lose_focus();
		m_input = m_objects.end();
		engine::play_sound(sound::CANCEL, 0.5f, 0.0f);
	}
}

//

void ui_manager::hide_all(ticks time)
{
	for (std::unique_ptr<widget>& p : m_objects) {
		p->hide(time);
	}
}

void ui_manager::release_graphical_resources()
{
	for (std::unique_ptr<widget>& p : m_objects) {
		p->release_graphical_resources();
	}
}

//

void ui_manager::handle_event(const tr::system::event& event)
{
	switch (event.type()) {
	case tr::system::mouse_motion_event::ID: {
		const std::list<std::unique_ptr<widget>>::iterator old_hovered_it{m_hovered};

		m_hovered = m_objects.end();
		for (std::list<std::unique_ptr<widget>>::iterator it = m_objects.begin(); it != m_objects.end(); ++it) {
			if ((*it)->hoverable() && tr::frect2{(*it)->tl(), (*it)->size()}.contains(engine::mouse_pos())) {
				if (m_hovered == m_objects.end() || !(*m_hovered)->active()) {
					m_hovered = it;
				}
			}
		}

		if (old_hovered_it != m_hovered) {
			if (old_hovered_it != m_objects.end()) {
				if (engine::held_buttons() == tr::system::mouse_button::LEFT && (*old_hovered_it)->active()) {
					(*old_hovered_it)->on_hold_transfer_out();
				}
				else {
					(*old_hovered_it)->on_unhover();
				}
			}
			if (m_hovered != m_objects.end()) {
				if (engine::held_buttons() == tr::system::mouse_button::LEFT && (*m_hovered)->active()) {
					(*m_hovered)->on_hold_transfer_in();
				}
				else {
					(*m_hovered)->on_hover();
				}
			}
		}
	} break;
	case tr::system::mouse_down_event::ID: {
		if (tr::system::mouse_down_event{event}.button == tr::system::mouse_button::LEFT) {
			const bool something_had_input_focus{m_input != m_objects.end()};
			if (something_had_input_focus) {
				tr::system::disable_text_input_events();
				(*m_input)->on_lose_focus();
				m_input = m_objects.end();
			}

			const std::list<std::unique_ptr<widget>>::iterator old_hovered_it{m_hovered};
			m_hovered = m_objects.end();
			for (std::list<std::unique_ptr<widget>>::iterator it = m_objects.begin(); it != m_objects.end(); ++it) {
				if ((*it)->hoverable() && tr::frect2{(*it)->tl(), (*it)->size()}.contains(engine::mouse_pos())) {
					if (m_hovered == m_objects.end() || !(*m_hovered)->active()) {
						m_hovered = it;
					}
				}
			}

			if (old_hovered_it != m_hovered) {
				if (old_hovered_it != m_objects.end()) {
					(*old_hovered_it)->on_unhover();
				}
				if (m_hovered != m_objects.end()) {
					(*m_hovered)->on_hover();
				}
			}

			if (m_hovered != m_objects.end() && (*m_hovered)->active()) {
				(*m_hovered)->on_hold_begin();
			}
			else if (something_had_input_focus) {
				engine::play_sound(sound::CANCEL, 0.5f, 0.0f);
			}
		}
	} break;
	case tr::system::mouse_up_event::ID: {
		const tr::system::mouse_up_event mouse_up{event};
		if (mouse_up.button == tr::system::mouse_button::LEFT) {
			if (m_hovered != m_objects.end() && (*m_hovered)->active()) {
				(*m_hovered)->on_hold_end();
				if ((*m_hovered)->writable()) {
					tr::system::enable_text_input_events();
					m_input = m_hovered;
					(*m_hovered)->on_gain_focus();
				}
			}
		}
	} break;
	case tr::system::key_down_event::ID: {
		const tr::system::key_down_event key_down{event};

		if (m_input != m_objects.end()) {
			if (key_down.key == tr::system::keycode::ESCAPE) {
				clear_input_focus();
			}
			else if (key_down.key == tr::system::keycode::TAB) {
				if (key_down.mods == tr::system::keymod::SHIFT) {
					move_input_focus_backward();
				}
				else {
					move_input_focus_forward();
				}
			}
			else if ((*m_input)->active()) {
				if (key_down.mods == tr::system::keymod::CTRL && key_down.key == tr::system::keycode::C) {
					(*m_input)->on_copy();
				}
				else if (key_down.mods == tr::system::keymod::CTRL && key_down.key == tr::system::keycode::X) {
					(*m_input)->on_copy();
					(*m_input)->on_clear();
				}
				else if (key_down.mods == tr::system::keymod::CTRL && key_down.key == tr::system::keycode::V) {
					(*m_input)->on_paste();
				}
				else if (key_down.key == tr::system::keycode::BACKSPACE || key_down.key == tr::system::keycode::DELETE) {
					if (key_down.mods & tr::system::keymod::CTRL) {
						(*m_input)->on_clear();
					}
					else {
						(*m_input)->on_erase();
					}
				}
				else if (key_down.key == tr::system::keycode::ENTER) {
					(*m_input)->on_enter();
				}
			}
		}
		else {
			if (key_down.key == tr::system::keycode::TAB) {
				if (key_down.mods == tr::system::keymod::SHIFT) {
					move_input_focus_backward();
				}
				else {
					move_input_focus_forward();
				}
			}
			else {
				for (std::unique_ptr<widget>& p : m_objects) {
					if (p->is_shortcut({key_down.key, key_down.mods})) {
						p->on_shortcut();
					}
				}
			}
		}
	} break;
	case tr::system::text_input_event::ID:
		if (m_input != m_objects.end() && (*m_input)->active()) {
			(*m_input)->on_write(tr::system::text_input_event{event}.text);
		}
		break;
	}
}

void ui_manager::update()
{
	for (std::unique_ptr<widget>& p : m_objects) {
		p->update();
	}

	if (m_input != m_objects.end() && !(*m_input)->active()) {
		clear_input_focus();
	}
}

void ui_manager::add_to_renderer()
{
	for (std::unique_ptr<widget>& p : m_objects) {
		p->add_to_renderer();
	}
	if (m_hovered != m_objects.end()) {
		if ((*m_hovered)->tooltip_cb) {
			const std::string tooltip{(*m_hovered)->tooltip_cb()};
			if (!tooltip.empty()) {
				engine::tooltip().add_to_renderer(tooltip);
			}
		}
	}
}