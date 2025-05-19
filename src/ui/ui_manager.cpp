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

void ui_manager::release_graphical_resources() noexcept
{
	for (auto& p : _objects) {
		p->release_graphical_resources();
	}
}

//

void ui_manager::handle_event(const tr::event& event)
{
	switch (event.type()) {
	case tr::mouse_motion_event::ID: {
		const tr::mouse_motion_event mouse_motion{event};
		const auto old_hovered_it{_hovered};

		for (_hovered = _objects.begin(); _hovered != _objects.end(); ++_hovered) {
			mousable* mp{dynamic_cast<mousable*>(_hovered->get())};
			if (mp != nullptr && tr::frect2{(*_hovered)->tl(), (*_hovered)->size()}.contains(engine::to_game_coords(mouse_motion.pos))) {
				break;
			}
		}

		if (old_hovered_it != _hovered) {
			if (old_hovered_it != _objects.end()) {
				mousable& old_hovered{*dynamic_cast<mousable*>(old_hovered_it->get())};
				if (tr::mouse::held(tr::mouse_button::LEFT) && old_hovered.holdable()) {
					old_hovered.on_hold_transfer_out();
				}
				else {
					old_hovered.on_unhover();
				}
			}
			if (_hovered != _objects.end()) {
				mousable& hovered{*dynamic_cast<mousable*>(_hovered->get())};
				if (tr::mouse::held(tr::mouse_button::LEFT) && hovered.holdable()) {
					hovered.on_hold_transfer_in();
				}
				else {
					hovered.on_hover();
				}
			}
		}
	} break;
	case tr::mouse_down_event::ID: {
		const tr::mouse_down_event mouse_down{event};
		if (mouse_down.button == tr::mouse_button::LEFT) {
			clear_input_focus();
			if (_hovered != _objects.end()) {
				mousable& hovered_mousable{*dynamic_cast<mousable*>(_hovered->get())};
				if (hovered_mousable.holdable()) {
					hovered_mousable.on_hold_begin();
				}
			}
		}
	} break;
	case tr::mouse_up_event::ID: {
		const tr::mouse_up_event mouse_down{event};
		if (mouse_down.button == tr::mouse_button::LEFT) {
			if (_hovered != _objects.end()) {
				mousable& hovered_mousable{*dynamic_cast<mousable*>(_hovered->get())};
				if (hovered_mousable.holdable()) {
					hovered_mousable.on_hold_end();
				}
			}
		}
	} break;
	case tr::key_down_event::ID: {
		const tr::key_down_event key_down{event};

		if (_input != _objects.end()) {
			writable& input{*dynamic_cast<writable*>(_input->get())};
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
				input.on_copy();
			}
			else if (key_down.mods & tr::keymods::CTRL && key_down.key == tr::keycode::V) {
				input.on_paste();
			}
			else if (key_down.key == tr::keycode::BACKSPACE || key_down.key == tr::keycode::DELETE) {
				if (key_down.mods & tr::keymods::SHIFT) {
					input.on_clear();
				}
				else {
					input.on_erase();
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
		if (mp != nullptr && !mp->tooltip.empty()) {
			engine::tooltip().add_to_renderer(mp->tooltip);
		}
	}
}