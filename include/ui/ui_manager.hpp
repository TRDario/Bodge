#pragma once
#include "widget_base.hpp"

using selection_tree_row = std::initializer_list<tag>;
using selection_tree = std::initializer_list<selection_tree_row>;
using shortcut_table = std::initializer_list<std::pair<const tr::sys::key_chord, tag>>;

class ui_manager {
  public:
	ui_manager(selection_tree selection_tree, shortcut_table shortcuts);

	widget& operator[](tag tag);
	const widget& operator[](tag tag) const;
	template <class T> T& as(tag tag);
	template <class T> const T& as(tag tag) const;

	template <class T, class... Args>
		requires(std::constructible_from<T, Args...>)
	T& emplace(tag tag, Args&&... args);
	void replace(std::unordered_map<tag, std::unique_ptr<widget>>&& widgets);

	void clear_selection();
	void select_widget(tag tag);
	void select_next_widget();

	void hide_all_widgets(ticks time);
	void release_graphical_resources();

	void handle_event(const tr::sys::event& event);
	void update();
	void add_to_renderer();

  private:
	using kv_pair = std::pair<const tag, std::unique_ptr<widget>>;
	struct selection_node {
		const selection_tree_row* row;
		const tag* tag;
	};

	selection_tree m_selection_tree;
	std::unordered_map<tag, std::unique_ptr<widget>> m_widgets;
	std::unordered_map<tr::sys::key_chord, tag> m_shortcuts;
	tr::opt_ref<kv_pair> m_hovered;
	tr::opt_ref<kv_pair> m_selection;

	void change_selection(tr::opt_ref<kv_pair> new_selection);
	selection_node find_in_selection_tree(tag tag) const;
	void select_first_widget();
	void select_last_widget();
	void select_prev_widget();
	void select_widget_above();
	void select_widget_below();
	void select_widget_to_the_left();
	void select_widget_to_the_right();
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <class T> T& ui_manager::as(tag tag)
{
	return (T&)((*this)[tag]);
}

template <class T> const T& ui_manager::as(tag tag) const
{
	return (const T&)((*this)[tag]);
}

template <class T, class... Args>
	requires(std::constructible_from<T, Args...>)
T& ui_manager::emplace(tag tag, Args&&... args)
{
	return (T&)(*m_widgets.emplace(tag, std::make_unique<T>(std::forward<Args>(args)...)).first->second);
}