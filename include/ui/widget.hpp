#pragma once
#include "interpolated.hpp"

// Base UI widget template.
class widget {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a widget.
	widget(std::string&& name, glm::vec2 pos, tr::align alignment) noexcept;
	// Virtual destructor.
	virtual ~widget() noexcept = default;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// The name of the widget.
	std::string name;
	// The position of the widget.
	interpolated_vec2 pos;
	// The alignment of the widget.
	tr::align align;

	///////////////////////////////////////////////////////////////// SIZE ////////////////////////////////////////////////////////////////

	// Gets the size of the widget.
	virtual glm::vec2 size() const = 0;
	// Gets the top-left corner of the widget.
	glm::vec2 tl() const noexcept;

	/////////////////////////////////////////////////////////////// OPACITY ///////////////////////////////////////////////////////////////

	// Gets the opacity of the widget.
	float opacity() const noexcept;
	// Hides the widget instantly.
	void hide() noexcept;
	// Hides the widget gradually.
	void hide(std::uint16_t time) noexcept;
	// Unhides the widget instantly.
	void unhide() noexcept;
	// Unhides the widget gradually.
	void unhide(std::uint16_t time) noexcept;

	////////////////////////////////////////////////////////////// UPDATING ///////////////////////////////////////////////////////////////

	// Updates the widget.
	virtual void update();
	// Adds the widget to the renderer.
	virtual void add_to_renderer() = 0;

  private:
	// The opacity of the widget.
	interpolated_float _opacity;
};

// Interface for a mousable widget.
struct mousable {
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a mousable without a tooltip.
	mousable() noexcept = default;
	// Creates a mousable with a tooltip.
	mousable(std::string&& tooltip) noexcept;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// The tooptip localization key, or an empty string for none.
	std::string tooltip;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Gets whether the widget is holdable.
	virtual bool holdable() const noexcept = 0;

	////////////////////////////////////////////////////////////// CALLBACKS //////////////////////////////////////////////////////////////

	// Callback for the widget being hovered.
	virtual void on_hover() = 0;
	// Callback for the widget being unhovered.
	virtual void on_unhover() = 0;
	// Callback for the widget being clicked and starting to be held.
	virtual void on_hold_begin() = 0;
	// Callback for the widget being held via hold transfer.
	virtual void on_hold_transfer_in() = 0;
	// Callback for the widget being unheld via hold transfer.
	virtual void on_hold_transfer_out() = 0;
	// Callback for the widget being unheld via click release.
	virtual void on_hold_end() = 0;
};

// Interface for a shortcutable widget.
struct shortcutable {
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a shortcutable.
	shortcutable(tr::keycode chord_key, tr::keymods chord_mods) noexcept;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// Shortcut chord key.
	tr::keycode chord_key;
	// Shortcut chord modifiers.
	tr::keymods chord_mods;

	////////////////////////////////////////////////////////////// CALLBACKS //////////////////////////////////////////////////////////////

	// Callback for the shortcut being inputted.
	virtual void on_shortcut() = 0;
};

// Interface for a writable widget.
struct writable {
	////////////////////////////////////////////////////////////// CALLBACKS //////////////////////////////////////////////////////////////

	// Callback for gaining input focus.
	virtual void on_gain_focus() = 0;
	// Callback for losing input focus.
	virtual void on_lose_focus() = 0;
	// Callback for character insertion.
	virtual void on_write(std::string_view input) = 0;
	// Callback for character deletion.
	virtual void on_erase() = 0;
	// Callback for input clearing.
	virtual void on_clear() = 0;
	// Callback for text copying.
	virtual void on_copy() = 0;
	// Callback for text pasting.
	virtual void on_paste() = 0;
};