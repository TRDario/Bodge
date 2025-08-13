#pragma once
#include "widget.hpp"

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
		return static_cast<T&>((*this)[tag]);
	}
	// Emplaces a new widget into the UI.
	template <class T, class... Args>
		requires(std::constructible_from<T, Args...>)
	T& emplace(tag tag, Args&&... args)
	{
		return *static_cast<T*>(m_widgets.emplace(tag, std::make_unique<T>(std::forward<Args>(args)...)).first->second.get());
	}

	///////////////////////////////////////////////////////////// INPUT FOCUS /////////////////////////////////////////////////////////////

	// Moves the input focus to the next writable widget.
	void move_input_focus_forward();
	// Moves the input focus to the previous writable widget.
	void move_input_focus_backward();
	// Removes the current input focus if there was one.
	void clear_input_focus();

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
};