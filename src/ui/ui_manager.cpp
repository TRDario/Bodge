#include "../../include/ui/ui_manager.hpp"
#include "../../include/engine.hpp"

//

void ui_manager::clear() noexcept
{
	_objects.clear();
	_hovered = _objects.end();
	_input = _objects.end();
}

//

void ui_manager::move_input_focus_forward() noexcept
{
	if (_input == _objects.end()) {
		for (std::list<std::unique_ptr<widget>>::iterator it = _objects.begin(); it != _objects.end(); ++it) {
			if ((*it)->writable()) {
				tr::event_queue::send_text_input_events(true);
				(*it)->on_gain_focus();
				_input = it;
				return;
			}
		}
	}
	else {
		// Search to the end of the vector.
		for (std::list<std::unique_ptr<widget>>::iterator it = std::next(_input); it != _objects.end(); ++it) {
			if ((*it)->writable()) {
				(*_input)->on_lose_focus();
				(*it)->on_gain_focus();
				_input = it;
				return;
			}
		}
		// Nothing else found, just unselect.
		clear_input_focus();
	}
}

void ui_manager::move_input_focus_backward() noexcept
{
	if (_input == _objects.end()) {
		for (auto it = _objects.rbegin(); it != _objects.rend(); ++it) {
			if ((*it)->writable()) {
				(*it)->on_gain_focus();
				_input = --(it.base());
				return;
			}
		}
	}
	else {
		// Search to the end of the vector.
		for (auto it = std::next(std::make_reverse_iterator(_input)); it != _objects.rend(); ++it) {
			if ((*it)->writable()) {
				(*it)->on_gain_focus();
				(*_input)->on_lose_focus();
				_input = --(it.base());
				return;
			}
		}
		// Nothing else found, just unselect.
		clear_input_focus();
	}
}

void ui_manager::clear_input_focus() noexcept
{
	if (_input != _objects.end()) {
		tr::event_queue::send_text_input_events(false);
		(*_input)->on_lose_focus();
		_input = _objects.end();
		audio::play(sfx::CANCEL, 0.5f, 0.0f);
	}
}

//

void ui_manager::hide_all(ticks time) noexcept
{
	for (std::unique_ptr<widget>& p : _objects) {
		p->hide(time);
	}
}

void ui_manager::release_graphical_resources() noexcept
{
	for (std::unique_ptr<widget>& p : _objects) {
		p->release_graphical_resources();
	}
}

//

void ui_manager::handle_event(const tr::event& event)
{
	switch (event.type()) {
	case tr::mouse_motion_event::ID: {
		const std::list<std::unique_ptr<widget>>::iterator old_hovered_it{_hovered};

		_hovered = _objects.end();
		for (std::list<std::unique_ptr<widget>>::iterator it = _objects.begin(); it != _objects.end(); ++it) {
			if ((*it)->hoverable() && tr::frect2{(*it)->tl(), (*it)->size()}.contains(engine::mouse_pos())) {
				if (_hovered == _objects.end() || !(*_hovered)->active()) {
					_hovered = it;
				}
			}
		}

		if (old_hovered_it != _hovered) {
			if (old_hovered_it != _objects.end()) {
				if (engine::held_buttons() == tr::mouse_button::LEFT && (*old_hovered_it)->active()) {
					(*old_hovered_it)->on_hold_transfer_out();
				}
				else {
					(*old_hovered_it)->on_unhover();
				}
			}
			if (_hovered != _objects.end()) {
				if (engine::held_buttons() == tr::mouse_button::LEFT && (*_hovered)->active()) {
					(*_hovered)->on_hold_transfer_in();
				}
				else {
					(*_hovered)->on_hover();
				}
			}
		}
	} break;
	case tr::mouse_down_event::ID: {
		if (tr::mouse_down_event{event}.button == tr::mouse_button::LEFT) {
			const bool something_had_input_focus{_input != _objects.end()};
			if (something_had_input_focus) {
				tr::event_queue::send_text_input_events(false);
				(*_input)->on_lose_focus();
				_input = _objects.end();
			}

			const std::list<std::unique_ptr<widget>>::iterator old_hovered_it{_hovered};
			_hovered = _objects.end();
			for (std::list<std::unique_ptr<widget>>::iterator it = _objects.begin(); it != _objects.end(); ++it) {
				if ((*it)->hoverable() && tr::frect2{(*it)->tl(), (*it)->size()}.contains(engine::mouse_pos())) {
					if (_hovered == _objects.end() || !(*_hovered)->active()) {
						_hovered = it;
					}
				}
			}

			if (old_hovered_it != _hovered) {
				if (old_hovered_it != _objects.end()) {
					(*old_hovered_it)->on_unhover();
				}
				if (_hovered != _objects.end()) {
					(*_hovered)->on_hover();
				}
			}

			if (_hovered != _objects.end() && (*_hovered)->active()) {
				(*_hovered)->on_hold_begin();
			}
			else if (something_had_input_focus) {
				audio::play(sfx::CANCEL, 0.5f, 0.0f);
			}
		}
	} break;
	case tr::mouse_up_event::ID: {
		const tr::mouse_up_event mouse_up{event};
		if (mouse_up.button == tr::mouse_button::LEFT) {
			if (_hovered != _objects.end() && (*_hovered)->active()) {
				(*_hovered)->on_hold_end();
				if ((*_hovered)->writable()) {
					tr::event_queue::send_text_input_events(true);
					_input = _hovered;
					(*_hovered)->on_gain_focus();
				}
			}
		}
	} break;
	case tr::key_down_event::ID: {
		const tr::key_down_event key_down{event};

		if (_input != _objects.end()) {
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
			else if ((*_input)->active()) {
				if (key_down.mods == tr::keymod::CTRL && key_down.key == tr::keycode::C) {
					(*_input)->on_copy();
				}
				else if (key_down.mods == tr::keymod::CTRL && key_down.key == tr::keycode::X) {
					(*_input)->on_copy();
					(*_input)->on_clear();
				}
				else if (key_down.mods == tr::keymod::CTRL && key_down.key == tr::keycode::V) {
					(*_input)->on_paste();
				}
				else if (key_down.key == tr::keycode::BACKSPACE || key_down.key == tr::keycode::DELETE) {
					if (key_down.mods & tr::keymod::CTRL) {
						(*_input)->on_clear();
					}
					else {
						(*_input)->on_erase();
					}
				}
				else if (key_down.key == tr::keycode::ENTER) {
					(*_input)->on_enter();
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
				for (std::unique_ptr<widget>& p : _objects) {
					if (p->is_shortcut({key_down.key, key_down.mods})) {
						p->on_shortcut();
					}
				}
			}
		}
	} break;
	case tr::text_input_event::ID:
		if (_input != _objects.end() && (*_input)->active()) {
			(*_input)->on_write(tr::text_input_event{event}.text);
		}
		break;
	}
}

void ui_manager::update() noexcept
{
	for (std::unique_ptr<widget>& p : _objects) {
		p->update();
	}

	if (_input != _objects.end() && !(*_input)->active()) {
		clear_input_focus();
	}
}

void ui_manager::add_to_renderer() noexcept
{
	for (std::unique_ptr<widget>& p : _objects) {
		p->add_to_renderer();
	}
	if (_hovered != _objects.end()) {
		if ((*_hovered)->tooltip_cb) {
			const std::string tooltip{(*_hovered)->tooltip_cb()};
			if (!tooltip.empty()) {
				engine::tooltip().add_to_renderer(tooltip);
			}
		}
	}
}