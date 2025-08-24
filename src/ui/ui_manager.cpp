#include "../../include/ui/ui_manager.hpp"
#include "../../include/audio.hpp"
#include "../../include/graphics.hpp"
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

void ui_manager::clear_selection()
{
	change_selection(std::nullopt);
}

void ui_manager::select_widget(tag tag)
{
	TR_ASSERT(m_widgets.contains(tag), "Tried to select nonexistant widget '{}'.", tag);
	TR_ASSERT(m_widgets[tag]->interactible(), "Tried to select non-interactible widget '{}'.", tag);

	return change_selection(*m_widgets.find(tag));
}

void ui_manager::change_selection(tr::opt_ref<kv_pair> new_selection)
{
	if (new_selection != m_selection) {
		if (m_selection.has_value()) {
			if (m_selection->second->writable()) {
				tr::system::disable_text_input_events();
			}
			m_selection->second->on_unselected();
		}
		if (new_selection.has_value()) {
			if (new_selection->second->writable()) {
				tr::system::enable_text_input_events();
			}
			new_selection->second->on_selected();
		}
		m_selection = new_selection;
		engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
	}
}

ui_manager::selection_node ui_manager::find_in_selection_tree(tag tag) const
{
	for (const selection_tree_row* row = m_selection_tree.begin(); row != m_selection_tree.end(); ++row) {
		for (const ::tag* it = row->begin(); it != row->end(); ++it) {
			if (*it == tag) {
				return {row, it};
			}
		}
	}

	TR_ASSERT(false, "Failed to find tag '{}' in the selection tree.", tag);
	return {};
}

void ui_manager::select_first_widget()
{
	for (const selection_tree_row& row : m_selection_tree) {
		for (tag tag : row) {
			auto it{m_widgets.find(tag)};
			if (it != m_widgets.end() && it->second->interactible()) {
				select_widget(it->first);
				return;
			}
		}
	}
}

void ui_manager::select_last_widget()
{
	for (const selection_tree_row& row : std::views::reverse(m_selection_tree)) {
		for (tag tag : std::views::reverse(row)) {
			auto it{m_widgets.find(tag)};
			if (it != m_widgets.end() && it->second->interactible()) {
				select_widget(it->first);
				return;
			}
		}
	}
}

void ui_manager::select_next_widget()
{
	if (!m_selection.has_value()) {
		select_first_widget();
	}
	else {
		auto [row_it, tag_it]{find_in_selection_tree(m_selection->first)};
		++tag_it;
		while (row_it != m_selection_tree.end()) {
			while (tag_it != row_it->end()) {
				auto it{m_widgets.find(*tag_it)};
				if (it->second->interactible()) {
					select_widget(it->first);
					return;
				}
				++tag_it;
			}
			++row_it;
			tag_it = row_it->begin();
		}
		select_first_widget();
	}
}

void ui_manager::select_prev_widget()
{
	if (!m_selection.has_value()) {
		select_last_widget();
	}
	else {
		auto [row_it, tag_it]{find_in_selection_tree(m_selection->first)};
		--tag_it;
		while (row_it >= m_selection_tree.begin()) {
			while (tag_it >= row_it->begin()) {
				auto it{m_widgets.find(*tag_it)};
				if (it->second->interactible()) {
					select_widget(it->first);
					return;
				}
				--tag_it;
			}
			--row_it;
			tag_it = std::prev(row_it->end());
		}
		select_last_widget();
	}
}

void ui_manager::select_widget_above()
{
	if (!m_selection.has_value()) {
		select_last_widget();
	}
	else {
		auto [row_it, tag_it]{find_in_selection_tree(m_selection->first)};
		const usize offset{usize(tag_it - row_it->begin())};
		--row_it;
		while (row_it >= m_selection_tree.begin()) {
			auto it{m_widgets.find(row_it->begin()[std::min(offset, row_it->size() - 1)])};
			if (it->second->interactible()) {
				select_widget(it->first);
				return;
			}

			for (tag tag : *row_it) {
				if (m_widgets.at(tag)->interactible()) {
					select_widget(tag);
					return;
				}
			}

			--row_it;
		}
		select_last_widget();
	}
}

void ui_manager::select_widget_below()
{
	if (!m_selection.has_value()) {
		select_first_widget();
	}
	else {
		auto [row_it, tag_it]{find_in_selection_tree(m_selection->first)};
		const usize offset{usize(tag_it - row_it->begin())};
		++row_it;
		while (row_it != m_selection_tree.end()) {
			auto it{m_widgets.find(row_it->begin()[std::min(offset, row_it->size() - 1)])};
			if (it->second->interactible()) {
				select_widget(it->first);
				return;
			}

			for (tag tag : *row_it) {
				if (m_widgets.at(tag)->interactible()) {
					select_widget(tag);
					return;
				}
			}

			++row_it;
		}
		select_first_widget();
	}
}

void ui_manager::select_widget_to_the_left()
{
	if (m_selection.has_value()) {
		auto [row_it, tag_it]{find_in_selection_tree(m_selection->first)};
		--tag_it;
		while (tag_it >= row_it->begin()) {
			auto it{m_widgets.find(*tag_it)};
			if (it->second->interactible()) {
				select_widget(it->first);
				return;
			}
			--tag_it;
		}
		select_widget(*std::prev(row_it->end()));
	}
}

void ui_manager::select_widget_to_the_right()
{
	if (m_selection.has_value()) {
		auto [row_it, tag_it]{find_in_selection_tree(m_selection->first)};
		++tag_it;
		while (tag_it != row_it->end()) {
			auto it{m_widgets.find(*tag_it)};
			if (it->second->interactible()) {
				select_widget(it->first);
				return;
			}
			++tag_it;
		}
		select_widget(*row_it->begin());
	}
}

//

void ui_manager::hide_all_widgets(ticks time)
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
		tr::opt_ref<kv_pair> new_hovered;
		for (kv_pair& kv : m_widgets) {
			if (!kv.second->hidden() && tr::frect2{kv.second->tl(), kv.second->size()}.contains(engine::mouse_pos())) {
				new_hovered = kv;
				break;
			}
		}

		if (m_hovered != new_hovered) {
			if (m_hovered.has_value()) {
				if (engine::held_buttons() == tr::system::mouse_button::LEFT && m_hovered->second->interactible()) {
					m_hovered->second->on_unheld();
				}
				m_hovered->second->on_unhover();
			}
			if (new_hovered.has_value()) {
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
			if (m_selection.has_value()) {
				tr::system::disable_text_input_events();
				m_selection->second->on_unselected();
				m_selection = std::nullopt;
			}

			tr::opt_ref<kv_pair> new_hovered;
			for (kv_pair& kv : m_widgets) {
				if (!kv.second->hidden() && tr::frect2{kv.second->tl(), kv.second->size()}.contains(engine::mouse_pos())) {
					new_hovered = kv;
					break;
				}
			}

			if (m_hovered != new_hovered) {
				if (m_hovered.has_value()) {
					m_hovered->second->on_unhover();
				}
				if (new_hovered.has_value()) {
					new_hovered->second->on_hover();
				}
				m_hovered = new_hovered;
			}

			if (m_hovered.has_value() && m_hovered->second->interactible()) {
				m_hovered->second->on_held();
			}
		}
	} break;
	case tr::system::mouse_up_event::ID: {
		const tr::system::mouse_up_event mouse_up{event};
		if (mouse_up.button == tr::system::mouse_button::LEFT) {
			if (m_hovered.has_value() && m_hovered->second->interactible()) {
				m_hovered->second->on_unheld();
				m_hovered->second->on_action();
				if (m_hovered->second->writable()) {
					tr::system::enable_text_input_events();
					m_selection = m_hovered;
					m_hovered->second->on_selected();
				}
			}
		}
	} break;
	case tr::system::key_down_event::ID: {
		const tr::system::key_down_event key_down{event};

		if (key_down.key == tr::system::keycode::TAB) {
			if (key_down.mods == tr::system::keymod::SHIFT) {
				select_prev_widget();
			}
			else {
				select_next_widget();
			}
		}
		else if (key_down == tr::system::key_chord{tr::system::keycode::UP}) {
			select_widget_above();
		}
		else if (key_down == tr::system::key_chord{tr::system::keycode::DOWN}) {
			select_widget_below();
		}
		else if (key_down == tr::system::key_chord{tr::system::keycode::LEFT} && !m_shortcuts.contains({tr::system::keycode::LEFT})) {
			select_widget_to_the_left();
		}
		else if (key_down == tr::system::key_chord{tr::system::keycode::RIGHT} && !m_shortcuts.contains({tr::system::keycode::RIGHT})) {
			select_widget_to_the_right();
		}
		else if (m_selection.has_value()) {
			if (key_down == tr::system::key_chord{tr::system::keycode::ESCAPE}) {
				clear_selection();
			}
			else if (m_selection->second->interactible()) {
				if (m_selection->second->writable()) {
					if (key_down == tr::system::key_chord{tr::system::keycode::C, tr::system::keymod::CTRL}) {
						m_selection->second->on_copy();
					}
					else if (key_down == tr::system::key_chord{tr::system::keycode::X, tr::system::keymod::CTRL}) {
						m_selection->second->on_copy();
						m_selection->second->on_clear();
					}
					else if (key_down == tr::system::key_chord{tr::system::keycode::V, tr::system::keymod::CTRL}) {
						m_selection->second->on_paste();
					}
					else if (key_down.key == tr::system::keycode::BACKSPACE || key_down.key == tr::system::keycode::DELETE) {
						if (key_down.mods == tr::system::keymod::CTRL) {
							m_selection->second->on_clear();
						}
						else {
							m_selection->second->on_erase();
						}
					}
					else if (key_down == tr::system::key_chord{tr::system::keycode::ENTER}) {
						m_selection->second->on_enter();
					}
				}
				else if (key_down == tr::system::key_chord{tr::system::keycode::ENTER}) {
					m_selection->second->on_action();
				}
			}
		}
		else {
			auto it{m_shortcuts.find({key_down.key, key_down.mods})};
			if (it != m_shortcuts.end()) {
				widget& widget{(*this)[it->second]};
				if (widget.interactible()) {
					widget.on_action();
					clear_selection();
				}
			}
		}
	} break;
	case tr::system::text_input_event::ID:
		if (m_selection.has_value() && m_selection->second->interactible() && m_selection->second->writable() &&
			!(engine::held_keymods() & tr::system::keymod::CTRL)) {
			m_selection->second->on_write(tr::system::text_input_event{event}.text);
		}
		break;
	}
}

void ui_manager::update()
{
	for (widget& widget : tr::deref(std::views::values(m_widgets))) {
		widget.update();
	}

	if (m_selection.has_value() && !m_selection->second->interactible()) {
		clear_selection();
	}
}

void ui_manager::add_to_renderer()
{
	for (widget& widget : tr::deref(std::views::values(m_widgets))) {
		widget.add_to_renderer();
	}
	if (m_hovered.has_value()) {
		if (m_hovered->second->tooltip_cb) {
			const std::string tooltip{m_hovered->second->tooltip_cb()};
			if (!tooltip.empty()) {
				engine::tooltip().add_to_renderer(tooltip);
			}
		}
	}
}