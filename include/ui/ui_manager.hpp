#pragma once
#include "widget.hpp"

// UI manager class.
class ui_manager {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates an empty UI manager.
	ui_manager() = default;

	///////////////////////////////////////////////////////// INSERTION AND ACCESS ////////////////////////////////////////////////////////

	// Emplaces a new widget into the UI.
	template <class T, class... Args>
		requires(std::constructible_from<T, Args...>)
	T& emplace(Args&&... args)
	{
		return *static_cast<T*>(m_objects.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)).get());
	}
	// Gets a widget by name.
	template <class T = widget> T& get(std::string_view name)
	{
		std::list<std::unique_ptr<widget>>::iterator it{
			std::ranges::find_if(m_objects, [=](std::unique_ptr<widget>& p) { return p->name == name; })};
		TR_ASSERT(it != m_objects.end(), "Tried to get widget with nonexistant name \"{}\".", name);
		return *static_cast<T*>(it->get());
	}
	// Clears the UI.
	void clear();

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
	// The list of widgets.
	std::list<std::unique_ptr<widget>> m_objects;
	// Iterator to the widget being hovered over, or end().
	std::list<std::unique_ptr<widget>>::iterator m_hovered{m_objects.end()};
	// Pointer to the widget with input focus, or end().
	std::list<std::unique_ptr<widget>>::iterator m_input{m_objects.end()};
};