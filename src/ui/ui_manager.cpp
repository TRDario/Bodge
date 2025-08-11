#include "../../include/ui/ui_manager.hpp"
#include "../../include/system.hpp"

ui_manager::ui_manager(shortcut_table shortcuts)
	: m_shortcuts{shortcuts}
{
}

//

widget& ui_manager::operator[](tag tag)
{
	TR_ASSERT(m_widgets.contains(tag), "Tried to get widget with nonexistant tag \"{}\".", tag);

	return *m_widgets.find(tag)->second;
}

//

void ui_manager::move_input_focus_forward() {}

void ui_manager::move_input_focus_backward() {}

void ui_manager::clear_input_focus()
{
	if (m_input != nullptr) {
		tr::system::disable_text_input_events();
		m_input->second->on_lose_focus();
		m_input = nullptr;
		engine::play_sound(sound::CANCEL, 0.5f, 0.0f);
	}
}

//

void ui_manager::hide_all(ticks time)
{
	for (widget& widget : tr::deref(std::views::values(m_widgets))) {
		widget.hide(time);
	}
}

void ui_manager::release_graphical_resources()
{
	for (widget& widget : tr::deref(std::views::values(m_widgets))) {
		widget.release_graphical_resources();
	}
}

//

void ui_manager::handle_event(const tr::system::event& event)
{
	switch (event.type()) {
	case tr::system::mouse_motion_event::ID: {
		kv_pair* const old_hovered{m_hovered};

		m_hovered = nullptr;
		for (kv_pair& kv : m_widgets) {
			if (kv.second->hoverable() && tr::frect2{kv.second->tl(), kv.second->size()}.contains(engine::mouse_pos())) {
				if (m_hovered == nullptr || !m_hovered->second->active()) {
					m_hovered = &kv;
				}
			}
		}

		if (old_hovered != m_hovered) {
			if (old_hovered != nullptr) {
				if (engine::held_buttons() == tr::system::mouse_button::LEFT && old_hovered->second->active()) {
					old_hovered->second->on_hold_transfer_out();
				}
				else {
					old_hovered->second->on_unhover();
				}
			}
			if (m_hovered != nullptr) {
				if (engine::held_buttons() == tr::system::mouse_button::LEFT && m_hovered->second->active()) {
					m_hovered->second->on_hold_transfer_in();
				}
				else {
					m_hovered->second->on_hover();
				}
			}
		}
	} break;
	case tr::system::mouse_down_event::ID: {
		if (tr::system::mouse_down_event{event}.button == tr::system::mouse_button::LEFT) {
			const bool something_had_input_focus{m_input != nullptr};
			if (something_had_input_focus) {
				tr::system::disable_text_input_events();
				m_input->second->on_lose_focus();
				m_input = nullptr;
			}

			kv_pair* const old_hovered{m_hovered};
			m_hovered = nullptr;
			for (kv_pair& kv : m_widgets) {
				if (kv.second->hoverable() && tr::frect2{kv.second->tl(), kv.second->size()}.contains(engine::mouse_pos())) {
					if (m_hovered == nullptr || !m_hovered->second->active()) {
						m_hovered = &kv;
					}
				}
			}

			if (old_hovered != m_hovered) {
				if (old_hovered != nullptr) {
					old_hovered->second->on_unhover();
				}
				if (m_hovered != nullptr) {
					m_hovered->second->on_hover();
				}
			}

			if (m_hovered != nullptr && m_hovered->second->active()) {
				m_hovered->second->on_hold_begin();
			}
			else if (something_had_input_focus) {
				engine::play_sound(sound::CANCEL, 0.5f, 0.0f);
			}
		}
	} break;
	case tr::system::mouse_up_event::ID: {
		const tr::system::mouse_up_event mouse_up{event};
		if (mouse_up.button == tr::system::mouse_button::LEFT) {
			if (m_hovered != nullptr && m_hovered->second->active()) {
				m_hovered->second->on_hold_end();
				if (m_hovered->second->writable()) {
					tr::system::enable_text_input_events();
					m_input = m_hovered;
					m_hovered->second->on_gain_focus();
				}
			}
		}
	} break;
	case tr::system::key_down_event::ID: {
		const tr::system::key_down_event key_down{event};

		if (m_input != nullptr) {
			if (key_down == tr::system::key_chord{tr::system::keycode::ESCAPE}) {
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
			else if (m_input->second->active()) {
				if (key_down == tr::system::key_chord{tr::system::keycode::C, tr::system::keymod::CTRL}) {
					m_input->second->on_copy();
				}
				else if (key_down == tr::system::key_chord{tr::system::keycode::X, tr::system::keymod::CTRL}) {
					m_input->second->on_copy();
					m_input->second->on_clear();
				}
				else if (key_down == tr::system::key_chord{tr::system::keycode::V, tr::system::keymod::CTRL}) {
					m_input->second->on_paste();
				}
				else if (key_down.key == tr::system::keycode::BACKSPACE || key_down.key == tr::system::keycode::DELETE) {
					if (key_down.mods == tr::system::keymod::CTRL) {
						m_input->second->on_clear();
					}
					else {
						m_input->second->on_erase();
					}
				}
				else if (key_down == tr::system::key_chord{tr::system::keycode::ENTER}) {
					m_input->second->on_enter();
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
				auto it{m_shortcuts.find({key_down.key, key_down.mods})};
				if (it != m_shortcuts.end()) {
					(*this)[it->second].on_shortcut();
				}
			}
		}
	} break;
	case tr::system::text_input_event::ID:
		if (m_input != nullptr && m_input->second->active() && !(engine::held_keymods() & tr::system::keymod::CTRL)) {
			m_input->second->on_write(tr::system::text_input_event{event}.text);
		}
		break;
	}
}

void ui_manager::update()
{
	for (widget& widget : tr::deref(std::views::values(m_widgets))) {
		widget.update();
	}

	if (m_input != nullptr && !m_input->second->active()) {
		clear_input_focus();
	}
}

void ui_manager::add_to_renderer()
{
	for (widget& widget : tr::deref(std::views::values(m_widgets))) {
		widget.add_to_renderer();
	}
	if (m_hovered != nullptr) {
		if (m_hovered->second->tooltip_cb) {
			const std::string tooltip{m_hovered->second->tooltip_cb()};
			if (!tooltip.empty()) {
				engine::tooltip().add_to_renderer(tooltip);
			}
		}
	}
}