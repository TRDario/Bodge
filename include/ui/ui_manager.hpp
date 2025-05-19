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
		return *static_cast<T*>(_objects.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)).get());
	}
	// Gets a widget by name.
	template <class T> T& get(std::string_view name) noexcept
	{
		auto it{std::ranges::find_if(_objects, [=](auto& p) { return p->name == name; })};
		TR_ASSERT(it != _objects.end(), "Tried to get widget with nonexistant name %.*s.", static_cast<int>(name.size()), name.data());
		return *static_cast<T*>(it->get());
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

	///////////////////////////////////////////////////////////// OPERATIONS //////////////////////////////////////////////////////////////

	// Handles an event.
	void handle_event(const tr::event& event);
	// Updates the UI.
	void update() noexcept;
	// Adds the UI to the renderer.
	void add_to_renderer() noexcept;

  private:
	// The list of widgets.
	std::list<std::unique_ptr<widget>> _objects;
	// Iterator to the widget being hovered over, or end().
	std::list<std::unique_ptr<widget>>::iterator _hovered{_objects.end()};
	// Pointer to the widget with input focus, or end().
	std::list<std::unique_ptr<widget>>::iterator _input{_objects.end()};
};