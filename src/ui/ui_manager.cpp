#include "../../include/engine.hpp"
#include "../../include/ui/ui_manager.hpp"

//

void ui_manager::clear()
{
	objects.clear();
	hovered = objects.end();
	input = objects.end();
}

//

void ui_manager::move_input_focus_forward()
{
	if (input == objects.end()) {
		for (std::list<std::unique_ptr<widget>>::iterator it = objects.begin(); it != objects.end(); ++it) {
			if ((*it)->writable()) {
				tr::event_queue::enable_text_input_events();
				(*it)->on_gain_focus();
				input = it;
				return;
			}
		}
	}
	else {
		// Search to the end of the vector.
		for (std::list<std::unique_ptr<widget>>::iterator it = std::next(input); it != objects.end(); ++it) {
			if ((*it)->writable()) {
				(*input)->on_lose_focus();
				(*it)->on_gain_focus();
				input = it;
				return;
			}
		}
		// Nothing else found, just unselect.
		clear_input_focus();
	}
}

void ui_manager::move_input_focus_backward()
{
	if (input == objects.end()) {
		for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
			if ((*it)->writable()) {
				(*it)->on_gain_focus();
				input = --(it.base());
				return;
			}
		}
	}
	else {
		// Search to the end of the vector.
		for (auto it = std::next(std::make_reverse_iterator(input)); it != objects.rend(); ++it) {
			if ((*it)->writable()) {
				(*it)->on_gain_focus();
				(*input)->on_lose_focus();
				input = --(it.base());
				return;
			}
		}
		// Nothing else found, just unselect.
		clear_input_focus();
	}
}

void ui_manager::clear_input_focus()
{
	if (input != objects.end()) {
		tr::event_queue::disable_text_input_events();
		(*input)->on_lose_focus();
		input = objects.end();
		audio::play_sound(sound::CANCEL, 0.5f, 0.0f);
	}
}

//

void ui_manager::hide_all(ticks time)
{
	for (std::unique_ptr<widget>& p : objects) {
		p->hide(time);
	}
}

void ui_manager::release_graphical_resources()
{
	for (std::unique_ptr<widget>& p : objects) {
		p->release_graphical_resources();
	}
}

//

void ui_manager::handle_event(const tr::event& event)
{
	switch (event.type()) {
	case tr::mouse_motion_event::ID: {
		const std::list<std::unique_ptr<widget>>::iterator old_hovered_it{hovered};

		hovered = objects.end();
		for (std::list<std::unique_ptr<widget>>::iterator it = objects.begin(); it != objects.end(); ++it) {
			if ((*it)->hoverable() && tr::frect2{(*it)->tl(), (*it)->size()}.contains(engine::mouse_pos())) {
				if (hovered == objects.end() || !(*hovered)->active()) {
					hovered = it;
				}
			}
		}

		if (old_hovered_it != hovered) {
			if (old_hovered_it != objects.end()) {
				if (engine::held_buttons() == tr::mouse_button::LEFT && (*old_hovered_it)->active()) {
					(*old_hovered_it)->on_hold_transfer_out();
				}
				else {
					(*old_hovered_it)->on_unhover();
				}
			}
			if (hovered != objects.end()) {
				if (engine::held_buttons() == tr::mouse_button::LEFT && (*hovered)->active()) {
					(*hovered)->on_hold_transfer_in();
				}
				else {
					(*hovered)->on_hover();
				}
			}
		}
	} break;
	case tr::mouse_down_event::ID: {
		if (tr::mouse_down_event{event}.button == tr::mouse_button::LEFT) {
			const bool something_had_input_focus{input != objects.end()};
			if (something_had_input_focus) {
				tr::event_queue::disable_text_input_events();
				(*input)->on_lose_focus();
				input = objects.end();
			}

			const std::list<std::unique_ptr<widget>>::iterator old_hovered_it{hovered};
			hovered = objects.end();
			for (std::list<std::unique_ptr<widget>>::iterator it = objects.begin(); it != objects.end(); ++it) {
				if ((*it)->hoverable() && tr::frect2{(*it)->tl(), (*it)->size()}.contains(engine::mouse_pos())) {
					if (hovered == objects.end() || !(*hovered)->active()) {
						hovered = it;
					}
				}
			}

			if (old_hovered_it != hovered) {
				if (old_hovered_it != objects.end()) {
					(*old_hovered_it)->on_unhover();
				}
				if (hovered != objects.end()) {
					(*hovered)->on_hover();
				}
			}

			if (hovered != objects.end() && (*hovered)->active()) {
				(*hovered)->on_hold_begin();
			}
			else if (something_had_input_focus) {
				audio::play_sound(sound::CANCEL, 0.5f, 0.0f);
			}
		}
	} break;
	case tr::mouse_up_event::ID: {
		const tr::mouse_up_event mouse_up{event};
		if (mouse_up.button == tr::mouse_button::LEFT) {
			if (hovered != objects.end() && (*hovered)->active()) {
				(*hovered)->on_hold_end();
				if ((*hovered)->writable()) {
					tr::event_queue::enable_text_input_events();
					input = hovered;
					(*hovered)->on_gain_focus();
				}
			}
		}
	} break;
	case tr::key_down_event::ID: {
		const tr::key_down_event key_down{event};

		if (input != objects.end()) {
			if (key_down.key == tr::keycode::ESCAPE) {
				clear_input_focus();
			}
			else if (key_down.key == tr::keycode::TAB) {
				if (key_down.mods == tr::keymod::SHIFT) {
					move_input_focus_backward();
				}
				else {
					move_input_focus_forward();
				}
			}
			else if ((*input)->active()) {
				if (key_down.mods == tr::keymod::CTRL && key_down.key == tr::keycode::C) {
					(*input)->on_copy();
				}
				else if (key_down.mods == tr::keymod::CTRL && key_down.key == tr::keycode::X) {
					(*input)->on_copy();
					(*input)->on_clear();
				}
				else if (key_down.mods == tr::keymod::CTRL && key_down.key == tr::keycode::V) {
					(*input)->on_paste();
				}
				else if (key_down.key == tr::keycode::BACKSPACE || key_down.key == tr::keycode::DELETE) {
					if (key_down.mods & tr::keymod::CTRL) {
						(*input)->on_clear();
					}
					else {
						(*input)->on_erase();
					}
				}
				else if (key_down.key == tr::keycode::ENTER) {
					(*input)->on_enter();
				}
			}
		}
		else {
			if (key_down.key == tr::keycode::TAB) {
				if (key_down.mods == tr::keymod::SHIFT) {
					move_input_focus_backward();
				}
				else {
					move_input_focus_forward();
				}
			}
			else {
				for (std::unique_ptr<widget>& p : objects) {
					if (p->is_shortcut({key_down.key, key_down.mods})) {
						p->on_shortcut();
					}
				}
			}
		}
	} break;
	case tr::text_input_event::ID:
		if (input != objects.end() && (*input)->active()) {
			(*input)->on_write(tr::text_input_event{event}.text);
		}
		break;
	}
}

void ui_manager::update()
{
	for (std::unique_ptr<widget>& p : objects) {
		p->update();
	}

	if (input != objects.end() && !(*input)->active()) {
		clear_input_focus();
	}
}

void ui_manager::add_to_renderer()
{
	for (std::unique_ptr<widget>& p : objects) {
		p->add_to_renderer();
	}
	if (hovered != objects.end()) {
		if ((*hovered)->tooltip_cb) {
			const std::string tooltip{(*hovered)->tooltip_cb()};
			if (!tooltip.empty()) {
				engine::tooltip().add_to_renderer(tooltip);
			}
		}
	}
}