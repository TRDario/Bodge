#include "../../include/ui/ui_manager.hpp"
#include "../../include/system.hpp"

ui_manager::ui_manager(selection_tree selection_tree, shortcut_table shortcuts)
	: m_selection_tree(selection_tree), m_shortcuts{shortcuts}
{
}

//

widget& ui_manager::operator[](tag tag)
{
	TR_ASSERT(m_widgets.contains(tag), "Tried to get widget with nonexistant tag \"{}\".", tag);

	return *m_widgets.find(tag)->second;
}

//

void ui_manager::set_selection(tag tag)
{
	kv_pair* old_selected{m_selected};

	if (tag == nullptr) {
		m_selected = nullptr;
	}
	else {
		TR_ASSERT(m_widgets.contains(tag), "Tried to select nonexistant widget '{}'.", tag);
		TR_ASSERT(m_widgets[tag]->interactible(), "Tried to select non-interactible widget '{}'.", tag);
		m_selected = &*m_widgets.find(tag);
	}

	if (m_selected != old_selected) {
		if (old_selected != nullptr) {
			if (old_selected->second->writable()) {
				tr::system::disable_text_input_events();
			}
			old_selected->second->on_unselected();
		}
		if (m_selected != nullptr) {
			if (m_selected->second->writable()) {
				tr::system::enable_text_input_events();
			}
			m_selected->second->on_selected();
		}
		engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
	}
}

void ui_manager::select_first()
{
	for (const selection_tree_row& row : m_selection_tree) {
		for (tag tag : row) {
			auto it{m_widgets.find(tag)};
			if (it != m_widgets.end() && it->second->interactible()) {
				set_selection(it->first);
				return;
			}
		}
	}
}

void ui_manager::select_last()
{
	for (const selection_tree_row& row : std::views::reverse(m_selection_tree)) {
		for (tag tag : std::views::reverse(row)) {
			auto it{m_widgets.find(tag)};
			if (it != m_widgets.end() && it->second->interactible()) {
				set_selection(it->first);
				return;
			}
		}
	}
}

void ui_manager::select_next()
{
	if (m_selected == nullptr) {
		select_first();
	}
	else {
		for (auto row_it = m_selection_tree.begin(); row_it != m_selection_tree.end(); ++row_it) {
			for (auto it = row_it->begin(); it != row_it->end(); ++it) {
				if (*it == m_selected->first) {
					if (std::next(it) != row_it->end()) {
						set_selection(*std::next(it));
					}
					else if (std::next(row_it) != m_selection_tree.end()) {
						set_selection(*std::next(row_it)->begin());
					}
					else {
						set_selection(*m_selection_tree.begin()->begin());
					}
					return;
				}
			}
		}
	}
}

void ui_manager::select_prev()
{
	if (m_selected == nullptr) {
		select_last();
	}
	else {
		const auto reverse_tree{std::views::reverse(m_selection_tree)};
		for (auto row_it = reverse_tree.begin(); row_it != reverse_tree.end(); ++row_it) {
			const auto reverse_row{std::views::reverse(*row_it)};
			for (auto it = reverse_row.begin(); it != reverse_row.end(); ++it) {
				if (*it == m_selected->first) {
					if (std::next(it) != reverse_row.end()) {
						set_selection(*std::next(it));
					}
					else if (std::next(row_it) != reverse_tree.end()) {
						set_selection(*std::next(row_it)->begin());
					}
					else {
						set_selection(*std::prev(std::prev(m_selection_tree.end())->end()));
					}
					return;
				}
			}
		}
	}
}

void ui_manager::select_up()
{
	if (m_selected == nullptr) {
		select_last();
	}
	else {
		const auto reverse_tree{std::views::reverse(m_selection_tree)};
		for (auto row_it = reverse_tree.begin(); row_it != reverse_tree.end(); ++row_it) {
			const auto reverse_row{std::views::reverse(*row_it)};
			for (auto it = reverse_row.begin(); it != reverse_row.end(); ++it) {
				if (*it == m_selected->first) {
					const std::size_t offset{static_cast<std::size_t>((it.base() - 1) - row_it->begin())};
					const selection_tree_row& prev_row{std::next(row_it) != reverse_tree.end() ? *std::next(row_it)
																							   : *std::prev(m_selection_tree.end())};
					set_selection(prev_row.begin()[std::min(offset, prev_row.size() - 1)]);
					return;
				}
			}
		}
	}
}

void ui_manager::select_down()
{
	if (m_selected == nullptr) {
		select_first();
	}
	else {
		for (auto row_it = m_selection_tree.begin(); row_it != m_selection_tree.end(); ++row_it) {
			for (auto it = row_it->begin(); it != row_it->end(); ++it) {
				if (*it == m_selected->first) {
					const std::size_t offset{static_cast<std::size_t>(it - row_it->begin())};
					const selection_tree_row& next_row{std::next(row_it) != m_selection_tree.end() ? *std::next(row_it)
																								   : *m_selection_tree.begin()};
					set_selection(next_row.begin()[std::min(offset, next_row.size() - 1)]);
					return;
				}
			}
		}
	}
}

void ui_manager::select_left()
{
	if (m_selected != nullptr) {
		const auto reverse_tree{std::views::reverse(m_selection_tree)};
		for (auto row_it = reverse_tree.begin(); row_it != reverse_tree.end(); ++row_it) {
			const auto reverse_row{std::views::reverse(*row_it)};
			for (auto it = reverse_row.begin(); it != reverse_row.end(); ++it) {
				if (*it == m_selected->first) {
					if (std::next(it) != reverse_row.end()) {
						set_selection(*std::next(it));
					}
					else {
						set_selection(*row_it->begin());
					}
					return;
				}
			}
		}
	}
}

void ui_manager::select_right()
{
	if (m_selected != nullptr) {
		for (auto row_it = m_selection_tree.begin(); row_it != m_selection_tree.end(); ++row_it) {
			for (auto it = row_it->begin(); it != row_it->end(); ++it) {
				if (*it == m_selected->first) {
					if (std::next(it) != row_it->end()) {
						set_selection(*std::next(it));
					}
					else {
						set_selection(*row_it->begin());
					}
					return;
				}
			}
		}
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
		kv_pair* new_hovered{nullptr};
		for (kv_pair& kv : m_widgets) {
			if (!kv.second->hidden() && tr::frect2{kv.second->tl(), kv.second->size()}.contains(engine::mouse_pos())) {
				new_hovered = &kv;
				break;
			}
		}

		if (m_hovered != new_hovered) {
			if (m_hovered != nullptr) {
				if (engine::held_buttons() == tr::system::mouse_button::LEFT && m_hovered->second->interactible()) {
					m_hovered->second->on_unheld();
				}
				m_hovered->second->on_unhover();
			}
			if (new_hovered != nullptr) {
				new_hovered->second->on_hover();
				if (engine::held_buttons() == tr::system::mouse_button::LEFT && new_hovered->second->interactible()) {
					new_hovered->second->on_held();
				}
			}
			m_hovered = new_hovered;
		}
	} break;
	case tr::system::mouse_down_event::ID: {
		if (tr::system::mouse_down_event{event}.button == tr::system::mouse_button::LEFT) {
			const bool something_had_input_focus{m_selected != nullptr};
			if (something_had_input_focus) {
				tr::system::disable_text_input_events();
				m_selected->second->on_unselected();
				m_selected = nullptr;
			}

			kv_pair* new_hovered{nullptr};
			for (kv_pair& kv : m_widgets) {
				if (!kv.second->hidden() && tr::frect2{kv.second->tl(), kv.second->size()}.contains(engine::mouse_pos())) {
					new_hovered = &kv;
					break;
				}
			}

			if (m_hovered != new_hovered) {
				if (m_hovered != nullptr) {
					m_hovered->second->on_unhover();
				}
				if (new_hovered != nullptr) {
					new_hovered->second->on_hover();
				}
				m_hovered = new_hovered;
			}

			if (m_hovered != nullptr && m_hovered->second->interactible()) {
				m_hovered->second->on_held();
			}
			else if (something_had_input_focus) {
				engine::play_sound(sound::CANCEL, 0.5f, 0.0f);
			}
		}
	} break;
	case tr::system::mouse_up_event::ID: {
		const tr::system::mouse_up_event mouse_up{event};
		if (mouse_up.button == tr::system::mouse_button::LEFT) {
			if (m_hovered != nullptr && m_hovered->second->interactible()) {
				m_hovered->second->on_unheld();
				m_hovered->second->on_action();
				if (m_hovered->second->writable()) {
					tr::system::enable_text_input_events();
					m_selected = m_hovered;
					m_hovered->second->on_selected();
				}
			}
		}
	} break;
	case tr::system::key_down_event::ID: {
		const tr::system::key_down_event key_down{event};

		if (key_down.key == tr::system::keycode::TAB) {
			if (key_down.mods == tr::system::keymod::SHIFT) {
				select_prev();
			}
			else {
				select_next();
			}
		}
		else if (key_down == tr::system::key_chord{tr::system::keycode::UP}) {
			select_up();
		}
		else if (key_down == tr::system::key_chord{tr::system::keycode::DOWN}) {
			select_down();
		}
		else if (key_down == tr::system::key_chord{tr::system::keycode::LEFT} && !m_shortcuts.contains({tr::system::keycode::LEFT})) {
			select_left();
		}
		else if (key_down == tr::system::key_chord{tr::system::keycode::RIGHT} && !m_shortcuts.contains({tr::system::keycode::RIGHT})) {
			select_right();
		}
		else if (m_selected != nullptr) {
			if (key_down == tr::system::key_chord{tr::system::keycode::ESCAPE}) {
				set_selection(nullptr);
			}
			else if (m_selected->second->interactible()) {
				if (m_selected->second->writable()) {
					if (key_down == tr::system::key_chord{tr::system::keycode::C, tr::system::keymod::CTRL}) {
						m_selected->second->on_copy();
					}
					else if (key_down == tr::system::key_chord{tr::system::keycode::X, tr::system::keymod::CTRL}) {
						m_selected->second->on_copy();
						m_selected->second->on_clear();
					}
					else if (key_down == tr::system::key_chord{tr::system::keycode::V, tr::system::keymod::CTRL}) {
						m_selected->second->on_paste();
					}
					else if (key_down.key == tr::system::keycode::BACKSPACE || key_down.key == tr::system::keycode::DELETE) {
						if (key_down.mods == tr::system::keymod::CTRL) {
							m_selected->second->on_clear();
						}
						else {
							m_selected->second->on_erase();
						}
					}
					else if (key_down == tr::system::key_chord{tr::system::keycode::ENTER}) {
						m_selected->second->on_enter();
					}
				}
				else if (key_down == tr::system::key_chord{tr::system::keycode::ENTER}) {
					m_selected->second->on_action();
				}
			}
		}
		else {
			auto it{m_shortcuts.find({key_down.key, key_down.mods})};
			if (it != m_shortcuts.end()) {
				widget& widget{(*this)[it->second]};
				if (widget.interactible()) {
					widget.on_action();
					set_selection(nullptr);
				}
			}
		}
	} break;
	case tr::system::text_input_event::ID:
		if (m_selected != nullptr && m_selected->second->interactible() && m_selected->second->writable() &&
			!(engine::held_keymods() & tr::system::keymod::CTRL)) {
			m_selected->second->on_write(tr::system::text_input_event{event}.text);
		}
		break;
	}
}

void ui_manager::update()
{
	for (widget& widget : tr::deref(std::views::values(m_widgets))) {
		widget.update();
	}

	if (m_selected != nullptr && !m_selected->second->interactible()) {
		set_selection(nullptr);
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