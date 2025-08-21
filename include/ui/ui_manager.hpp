#pragma once
#include "widget_base.hpp"

// List of widgets considered to be on the same row.
using selection_tree_row = std::initializer_list<const char*>;
// Widget selection tree (for keyboard navigation).
using selection_tree = std::initializer_list<selection_tree_row>;

// Table of widget shortcuts.
using shortcut_table = std::initializer_list<std::pair<const tr::system::key_chord, tag>>;

// UI manager class.
class ui_manager {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a UI manager.
	ui_manager(selection_tree selection_tree, shortcut_table shortcuts);

	///////////////////////////////////////////////////////// INSERTION AND ACCESS ////////////////////////////////////////////////////////

	// Gets a widget by tag.
	widget& operator[](tag tag);
	// Gets a widget by name.
	template <class T> T& as(tag tag)
	{
		return (T&)((*this)[tag]);
	}
	// Emplaces a new widget into the UI.
	template <class T, class... Args>
		requires(std::constructible_from<T, Args...>)
	T& emplace(tag tag, Args&&... args)
	{
		return *((T*)(m_widgets.emplace(tag, std::make_unique<T>(std::forward<Args>(args)...)).first->second.get()));
	}

	///////////////////////////////////////////////////////////// INPUT FOCUS /////////////////////////////////////////////////////////////

	// Sets the selection to a specific widget, or nullptr to unselect the currently-selected widget.
	void set_selection(tag tag);
	// Selects the next widget.
	void select_next();

	///////////////////////////////////////////////////////////// MASS ACTION /////////////////////////////////////////////////////////////

	// Hides all widgets.
	void hide_all(ticks time);
	// Instructs all widgets to release their graphical resources in service of a pending engine restart.
	void release_graphical_resources();

	///////////////////////////////////////////////////////////// OPERATIONS //////////////////////////////////////////////////////////////

	// Handles an event.
	void handle_event(const tr::system::event& event);
	// Updates the UI.
	void update();
	// Adds the UI to the renderer.
	void add_to_renderer();

  private:
	// Tag-widget pair.
	using kv_pair = std::pair<const tag, std::unique_ptr<widget>>;
	// Selection tree pair returned from find_in_selection_tree.
	struct selection_tree_pair {
		const selection_tree_row* row;
		const tag* tag;
	};

	// The selection tree.
	selection_tree m_selection_tree;
	// The widget list.
	std::unordered_map<tag, std::unique_ptr<widget>> m_widgets;
	// Shortcut table.
	std::unordered_map<tr::system::key_chord, tag> m_shortcuts;
	// Pointer to the widget being hovered over, or nullptr.
	kv_pair* m_hovered{nullptr};
	// Pointer to the widget with input focus, or nullptr.
	kv_pair* m_selected{nullptr};

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Finds the tag in the selection tree.
	selection_tree_pair find_in_selection_tree(tag tag) const;
	// Selects the first widget.
	void select_first();
	// Selects the last widget.
	void select_last();
	// Selects the previous widget.
	void select_prev();
	// Selects the widget above.
	void select_up();
	// Selects the widget below.
	void select_down();
	// Selects the widget to the left.
	void select_left();
	// Select the widget to the right.
	void select_right();
};