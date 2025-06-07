#include "../../include/engine.hpp"
#include "../../include/ui/ui_manager.hpp"

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
		for (list<unique_ptr<widget>>::iterator it = _objects.begin(); it != _objects.end(); ++it) {
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
		for (list<unique_ptr<widget>>::iterator it = std::next(_input); it != _objects.end(); ++it) {
			if ((*it)->writable()) {
				(*_input)->on_lose_focus();
				(*it)->on_gain_focus();
				_input = it;
				return;
			}
		}
		// Loop back and search up to the original.
		for (list<unique_ptr<widget>>::iterator it = _objects.begin(); it != _input; ++it) {
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
		// Loop back and search up to the original.
		for (auto it = _objects.rbegin(); it != std::make_reverse_iterator(_input); ++it) {
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
	}
}

//

void ui_manager::hide_all(ticks time) noexcept
{
	for (unique_ptr<widget>& p : _objects) {
		p->hide(time);
	}
}

void ui_manager::release_graphical_resources() noexcept
{
	for (unique_ptr<widget>& p : _objects) {
		p->release_graphical_resources();
	}
}

//

void ui_manager::handle_event(const tr::event& event)
{
	switch (event.type()) {
	case tr::mouse_motion_event::ID: {
		const tr::mouse_motion_event mouse_motion{event};
		const list<unique_ptr<widget>>::iterator old_hovered_it{_hovered};

		_hovered = _objects.end();
		for (list<unique_ptr<widget>>::iterator it = _objects.begin(); it != _objects.end(); ++it) {
			if ((*it)->hoverable() && frect2{(*it)->tl(), (*it)->size()}.contains(engine::to_game_coords(mouse_motion.pos))) {
				if (_hovered == _objects.end() || !(*_hovered)->active()) {
					_hovered = it;
				}
			}
		}

		if (old_hovered_it != _hovered) {
			if (old_hovered_it != _objects.end()) {
				if (tr::mouse::held(tr::mouse_button::LEFT) && (*old_hovered_it)->active()) {
					(*old_hovered_it)->on_hold_transfer_out();
				}
				else {
					(*old_hovered_it)->on_unhover();
				}
			}
			if (_hovered != _objects.end()) {
				if (tr::mouse::held(tr::mouse_button::LEFT) && (*_hovered)->active()) {
					(*_hovered)->on_hold_transfer_in();
				}
				else {
					(*_hovered)->on_hover();
				}
			}
		}
	} break;
	case tr::mouse_down_event::ID: {
		const tr::mouse_down_event mouse_down{event};
		if (mouse_down.button == tr::mouse_button::LEFT) {
			clear_input_focus();

			const list<unique_ptr<widget>>::iterator old_hovered_it{_hovered};
			_hovered = _objects.end();
			for (list<unique_ptr<widget>>::iterator it = _objects.begin(); it != _objects.end(); ++it) {
				if ((*it)->hoverable() && frect2{(*it)->tl(), (*it)->size()}.contains(engine::to_game_coords(mouse_down.pos))) {
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
			if (key_down.key == key::ESCAPE) {
				clear_input_focus();
			}
			else if (key_down.key == key::TAB) {
				if (key_down.mods == mods::SHIFT) {
					move_input_focus_backward();
				}
				else {
					move_input_focus_forward();
				}
			}
			else if ((*_input)->active()) {
				if (key_down.mods == mods::CTRL && key_down.key == key::C) {
					(*_input)->on_copy();
				}
				else if (key_down.mods == mods::CTRL && key_down.key == key::V) {
					(*_input)->on_paste();
				}
				else if (key_down.key == key::BACKSPACE || key_down.key == key::DELETE) {
					if (key_down.mods == mods::SHIFT) {
						(*_input)->on_clear();
					}
					else {
						(*_input)->on_erase();
					}
				}
				else if (key_down.key == key::ENTER) {
					(*_input)->on_enter();
				}
			}
		}
		else {
			if (key_down.key == key::TAB) {
				if (key_down.mods == mods::SHIFT) {
					move_input_focus_backward();
				}
				else {
					move_input_focus_forward();
				}
			}
			else {
				for (unique_ptr<widget>& p : _objects) {
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
	for (unique_ptr<widget>& p : _objects) {
		p->update();
	}

	if (_input != _objects.end() && !(*_input)->active()) {
		clear_input_focus();
	}
}

void ui_manager::add_to_renderer() noexcept
{
	for (unique_ptr<widget>& p : _objects) {
		p->add_to_renderer();
	}
	if (_hovered != _objects.end()) {
		if ((*_hovered)->tooltip_cb) {
			const string tooltip{(*_hovered)->tooltip_cb()};
			if (!tooltip.empty()) {
				engine::tooltip().add_to_renderer(tooltip);
			}
		}
	}
}