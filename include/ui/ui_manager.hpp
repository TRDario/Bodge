#pragma once
#include "widget.hpp"

// UI manager class.
class ui_manager {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates an empty UI manager.
	ui_manager() noexcept = default;

	///////////////////////////////////////////////////////// INSERTION AND ACCESS ////////////////////////////////////////////////////////

	// Emplaces a new widget into the UI.
	template <class T, class... Args> T& emplace(Args&&... args)
	{
		return *static_cast<T*>(_objects.emplace_back(make_unique<T>(std::forward<Args>(args)...)).get());
	}
	// Gets a widget by name.
	template <class T = widget> T& get(string_view name) noexcept
	{
		list<unique_ptr<widget>>::iterator it{rs::find_if(_objects, [=](unique_ptr<widget>& p) { return p->name == name; })};
		TR_ASSERT(it != _objects.end(), "Tried to get widget with nonexistant name \"{}\".", name);
		return *dynamic_cast<T*>(it->get());
	}
	// Clears the UI.
	void clear() noexcept;

	///////////////////////////////////////////////////////////// INPUT FOCUS /////////////////////////////////////////////////////////////

	// Moves the input focus to the next writable widget.
	void move_input_focus_forward() noexcept;
	// Moves the input focus to the previous writable widget.
	void move_input_focus_backward() noexcept;
	// Removes the current input focus if there was one.
	void clear_input_focus() noexcept;

	///////////////////////////////////////////////////////////// MASS ACTION /////////////////////////////////////////////////////////////

	// Hides all widgets.
	void hide_all(ticks time) noexcept;
	// Instructs all widgets to release their graphical resources in service of a pending engine restart.
	void release_graphical_resources() noexcept;

	///////////////////////////////////////////////////////////// OPERATIONS //////////////////////////////////////////////////////////////

	// Handles an event.
	void handle_event(const tr::event& event);
	// Updates the UI.
	void update() noexcept;
	// Adds the UI to the renderer.
	void add_to_renderer() noexcept;

  private:
	// The list of widgets.
	list<unique_ptr<widget>> _objects;
	// Iterator to the widget being hovered over, or end().
	list<unique_ptr<widget>>::iterator _hovered{_objects.end()};
	// Pointer to the widget with input focus, or end().
	list<unique_ptr<widget>>::iterator _input{_objects.end()};
};