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
		for (auto it = _objects.begin(); it != _objects.end(); ++it) {
			writable* wp{dynamic_cast<writable*>(it->get())};
			if (wp != nullptr) {
				tr::event_queue().send_text_input_events(true);
				wp->on_gain_focus();
				_input = it;
				return;
			}
		}
	}
	else {
		// Search to the end of the vector.
		for (auto it = std::next(_input); it != _objects.end(); ++it) {
			writable* wp{dynamic_cast<writable*>(it->get())};
			if (wp != nullptr) {
				wp->on_gain_focus();
				dynamic_cast<writable*>(_input->get())->on_lose_focus();
				_input = it;
				return;
			}
		}
		// Loop back and search up to the original.
		for (auto it = _objects.begin(); it != _input; ++it) {
			writable* wp{dynamic_cast<writable*>(it->get())};
			if (wp != nullptr) {
				wp->on_gain_focus();
				dynamic_cast<writable*>(_input->get())->on_lose_focus();
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
			writable* wp{dynamic_cast<writable*>(it->get())};
			if (wp != nullptr) {
				wp->on_gain_focus();
				_input = --(it.base());
				return;
			}
		}
	}
	else {
		// Search to the end of the vector.
		for (auto it = std::next(std::make_reverse_iterator(_input)); it != _objects.rend(); ++it) {
			writable* wp{dynamic_cast<writable*>(it->get())};
			if (wp != nullptr) {
				wp->on_gain_focus();
				dynamic_cast<writable*>(_input->get())->on_lose_focus();
				_input = --(it.base());
				return;
			}
		}
		// Loop back and search up to the original.
		for (auto it = _objects.rbegin(); it != std::make_reverse_iterator(_input); ++it) {
			writable* wp{dynamic_cast<writable*>(it->get())};
			if (wp != nullptr) {
				wp->on_gain_focus();
				dynamic_cast<writable*>(_input->get())->on_lose_focus();
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
		dynamic_cast<writable*>(_input->get())->on_lose_focus();
		_input = _objects.end();
		tr::event_queue().send_text_input_events(false);
	}
}

//

void ui_manager::hide_all(ticks time) noexcept
{
	for (auto& p : _objects) {
		p->hide(time);
	}
}

//

void ui_manager::handle_event(const tr::event& event)
{
	auto IS_WRITABLE{[](auto& p) { return dynamic_cast<writable*>(p.get()) != nullptr; }};

	switch (event.type()) {
	case tr::key_down_event::ID: {
		const tr::key_down_event key_down{event};

		if (_input != _objects.end()) {
			if (key_down.key == tr::keycode::ESCAPE) {
				clear_input_focus();
			}
			else if (key_down.key == tr::keycode::TAB) {
				if (key_down.mods & tr::keymods::SHIFT) {
					move_input_focus_backward();
				}
				else {
					move_input_focus_forward();
				}
			}
			else if (key_down.mods & tr::keymods::CTRL && key_down.key == tr::keycode::C) {
				dynamic_cast<writable*>(_input->get())->on_copy();
			}
			else if (key_down.mods & tr::keymods::CTRL && key_down.key == tr::keycode::V) {
				dynamic_cast<writable*>(_input->get())->on_paste();
			}
			else if (key_down.key == tr::keycode::BACKSPACE || key_down.key == tr::keycode::DELETE) {
				if (key_down.mods & tr::keymods::SHIFT) {
					dynamic_cast<writable*>(_input->get())->on_clear();
				}
				else {
					dynamic_cast<writable*>(_input->get())->on_erase();
				}
			}
		}
		else {
			if (key_down.key == tr::keycode::TAB) {
				if (key_down.mods & tr::keymods::SHIFT) {
					move_input_focus_backward();
				}
				else {
					move_input_focus_forward();
				}
			}
			else {
				for (auto& p : _objects) {
					shortcutable* sp{dynamic_cast<shortcutable*>(p.get())};
					if (sp != nullptr && sp->chord_key == key_down.key && sp->chord_mods == key_down.mods) {
						sp->on_shortcut();
					}
				}
			}
		}
	} break;
	case tr::text_input_event::ID:
		dynamic_cast<writable*>(_input->get())->on_write(tr::text_input_event{event}.text);
		break;
	}
}

void ui_manager::update() noexcept
{
	for (auto& p : _objects) {
		p->update();
	}
}

void ui_manager::add_to_renderer() noexcept
{
	for (auto& p : _objects) {
		p->add_to_renderer();
	}
	if (_hovered != _objects.end()) {
		mousable* mp{dynamic_cast<mousable*>(_hovered->get())};
		if (mp != nullptr) {
			engine::tooltip().add_to_renderer(mp->tooltip);
		}
	}
}