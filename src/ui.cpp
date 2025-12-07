#include "../include/ui.hpp"
#include "../include/audio.hpp"
#include "../include/renderer.hpp"

/////////////////////////////////////////////////////////////// UI MANAGER ////////////////////////////////////////////////////////////////

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

void ui_manager::replace(std::unordered_map<tag, std::unique_ptr<widget>>&& widgets)
{
	for (auto& [tag, widget] : widgets) {
		m_widgets.insert_or_assign(tag, std::move(widget));
	}
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
		if (m_selection.has_ref()) {
			if (m_selection->second->writable()) {
				tr::sys::disable_text_input_events();
			}
			m_selection->second->on_unselected();
		}
		if (new_selection.has_ref()) {
			if (new_selection->second->writable()) {
				tr::sys::enable_text_input_events();
			}
			new_selection->second->on_selected();
		}
		m_selection = new_selection;
		g_audio.play_sound(sound::HOVER, 0.15f, 0.0f, g_rng.generate(0.9f, 1.1f));
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
	if (!m_selection.has_ref()) {
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
	if (!m_selection.has_ref()) {
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
	if (!m_selection.has_ref()) {
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
	if (!m_selection.has_ref()) {
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
	if (m_selection.has_ref()) {
		auto [row_it, cur_tag_it]{find_in_selection_tree(m_selection->first)};
		for (auto tag_it = cur_tag_it; tag_it != std::next(cur_tag_it); --tag_it) {
			if (tag_it == row_it->begin()) {
				tag_it = row_it->end();
			}

			auto it{m_widgets.find(tag_it[-1])};
			if (it->second->interactible()) {
				select_widget(it->first);
				return;
			}
		}
	}
}

void ui_manager::select_widget_to_the_right()
{
	if (m_selection.has_ref()) {
		auto [row_it, cur_tag_it]{find_in_selection_tree(m_selection->first)};
		auto tag_it{std::next(cur_tag_it)};
		for (tag_it = std::next(cur_tag_it); tag_it != cur_tag_it; ++tag_it) {
			if (tag_it == row_it->end()) {
				tag_it = row_it->begin();
			}

			auto it{m_widgets.find(*tag_it)};
			if (it->second->interactible()) {
				select_widget(it->first);
				return;
			}
		}
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
	std::ranges::for_each(tr::deref(std::views::values(m_widgets)), &widget::release_graphical_resources);
}

//

void ui_manager::handle_mouse_motion_event()
{
	tr::opt_ref<kv_pair> new_hovered;
	for (kv_pair& kv : m_widgets) {
		const tr::frect2 hitbox{kv.second->tl(), kv.second->size()};
		if (!kv.second->hidden() && hitbox.contains(g_mouse_pos)) {
			new_hovered = kv;
			break;
		}
	}

	if (m_hovered != new_hovered) {
		if (m_hovered.has_ref()) {
			if (g_held_buttons == tr::sys::mouse_button::LEFT && m_hovered->second->interactible()) {
				m_hovered->second->on_unheld();
			}
			m_hovered->second->on_unhover();
		}
		if (new_hovered.has_ref()) {
			new_hovered->second->on_hover();
			if (g_held_buttons == tr::sys::mouse_button::LEFT && new_hovered->second->interactible()) {
				new_hovered->second->on_held();
			}
		}
		m_hovered = new_hovered;
	}
}

void ui_manager::handle_mouse_down_event(const tr::sys::mouse_down_event& event)
{
	if (event.button == tr::sys::mouse_button::LEFT) {
		if (m_selection.has_ref()) {
			tr::sys::disable_text_input_events();
			m_selection->second->on_unselected();
			m_selection = std::nullopt;
		}

		tr::opt_ref<kv_pair> new_hovered;
		for (kv_pair& kv : m_widgets) {
			const tr::frect2 hitbox{kv.second->tl(), kv.second->size()};
			if (!kv.second->hidden() && hitbox.contains(g_mouse_pos)) {
				new_hovered = kv;
				break;
			}
		}

		if (m_hovered != new_hovered) {
			if (m_hovered.has_ref()) {
				m_hovered->second->on_unhover();
			}
			if (new_hovered.has_ref()) {
				new_hovered->second->on_hover();
			}
			m_hovered = new_hovered;
		}

		if (m_hovered.has_ref() && m_hovered->second->interactible()) {
			m_hovered->second->on_held();
		}
	}
}

void ui_manager::handle_mouse_up_event(const tr::sys::mouse_up_event& event)
{
	if (event.button == tr::sys::mouse_button::LEFT) {
		if (m_hovered.has_ref() && m_hovered->second->interactible()) {
			m_hovered->second->on_unheld();
			m_hovered->second->on_action();
			if (m_hovered->second->writable()) {
				tr::sys::enable_text_input_events();
				m_selection = m_hovered;
				m_hovered->second->on_selected();
			}
		}
	}
}

void ui_manager::handle_key_down_event(const tr::sys::key_down_event& event)
{
	if (event.key == "Tab"_k) {
		if (event.mods == tr::sys::keymod::SHIFT) {
			select_prev_widget();
		}
		else {
			select_next_widget();
		}
	}
	else if (event == "Up"_kc) {
		select_widget_above();
	}
	else if (event == "Down"_kc) {
		select_widget_below();
	}
	else if (event == "Left"_kc && !m_shortcuts.contains("Left"_kc)) {
		select_widget_to_the_left();
	}
	else if (event == "Right"_kc && !m_shortcuts.contains("Right"_kc)) {
		select_widget_to_the_right();
	}
	else if (m_selection.has_ref()) {
		if (event == "Escape"_kc) {
			clear_selection();
		}
		else if (m_selection->second->interactible()) {
			if (m_selection->second->writable()) {
				if (event == "Ctrl+C"_kc) {
					m_selection->second->on_copy();
				}
				else if (event == "Ctrl+X"_kc) {
					m_selection->second->on_copy();
					m_selection->second->on_clear();
				}
				else if (event == "Ctrl+V"_kc) {
					m_selection->second->on_paste();
				}
				else if (event.key == "Backspace"_k || event.key == "Delete"_k) {
					if (event.mods == tr::sys::keymod::CTRL) {
						m_selection->second->on_clear();
					}
					else {
						m_selection->second->on_erase();
					}
				}
				else if (event == "Enter"_kc) {
					m_selection->second->on_enter();
				}
			}
			else if (event == "Enter"_kc) {
				m_selection->second->on_action();
			}
		}
	}
	else {
		auto it{m_shortcuts.find({event.mods, event.key})};
		if (it != m_shortcuts.end()) {
			widget& widget{(*this)[it->second]};
			if (widget.interactible()) {
				widget.on_action();
				clear_selection();
			}
		}
	}
}

void ui_manager::handle_text_input_event(const tr::sys::text_input_event& event)
{
	if (m_selection.has_ref() && m_selection->second->interactible() && m_selection->second->writable() &&
		!(g_held_keymods & tr::sys::keymod::CTRL)) {
		m_selection->second->on_write(event.text);
	}
}

void ui_manager::handle_event(const tr::sys::event& event)
{
	if (event.is<tr::sys::mouse_motion_event>()) {
		handle_mouse_motion_event();
	}
	else if (event.is<tr::sys::mouse_down_event>()) {
		handle_mouse_down_event(event.as<tr::sys::mouse_down_event>());
	}
	else if (event.is<tr::sys::mouse_up_event>()) {
		handle_mouse_up_event(event.as<tr::sys::mouse_up_event>());
	}
	else if (event.is<tr::sys::key_down_event>()) {
		handle_key_down_event(event.as<tr::sys::key_down_event>());
	}
	else if (event.is<tr::sys::text_input_event>()) {
		handle_text_input_event(event.as<tr::sys::text_input_event>());
	}
}

void ui_manager::tick()
{
	std::ranges::for_each(tr::deref(std::views::values(m_widgets)), &widget::tick);
	if (m_selection.has_ref() && !m_selection->second->interactible()) {
		clear_selection();
	}
}

void ui_manager::add_to_renderer()
{
	std::ranges::for_each(tr::deref(std::views::values(m_widgets)), &widget::add_to_renderer);
	if (m_hovered.has_ref()) {
		if (m_hovered->second->tooltip_cb) {
			const std::string tooltip{m_hovered->second->tooltip_cb()};
			if (!tooltip.empty()) {
				g_renderer->tooltip.add(tooltip);
			}
		}
	}
}