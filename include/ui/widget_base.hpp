#pragma once
#include "interpolator.hpp"

////////////////////////////////////////////////////////////////// TYPES //////////////////////////////////////////////////////////////////

// Alias for a widget text callback.
using text_callback = std::function<std::string()>;
// Sentinel for a mousable to not have a tooltip.
inline const text_callback NO_TOOLTIP{};
// Common callback for text widgets: getting a localization std::string using the widget name as the key.
struct loc_text_callback {
	tag tag;

	std::string operator()() const;
};
// Common callback for text widgets: copying a string directly.
struct string_text_callback {
	std::string str;

	std::string operator()() const;
};

// Alias for a clickable widget status callback.
using status_callback = std::function<bool()>;
// Alias for a clickable widget action callback.
using action_callback = std::function<void()>;

// Sentinel to not unhide a widget.
constexpr ticks DONT_UNHIDE{std::numeric_limits<ticks>::max()};

////////////////////////////////////////////////////////////////// WIDGET /////////////////////////////////////////////////////////////////

// Base widget class.
class widget {
  public:
	// Creates a widget.
	widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, bool hoverable, text_callback tooltip_cb, bool writable);
	// Virtual destructor.
	virtual ~widget() = default;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// The alignment of the widget.
	tr::align alignment;
	// The position of the widget.
	interpolator<glm::vec2> pos;
	// An optional tooltip callback (the widget must be hoverable for it to be used).
	text_callback tooltip_cb;

	///////////////////////////////////////////////////////////////// SIZE ////////////////////////////////////////////////////////////////

	// Gets the size of the widget.
	virtual glm::vec2 size() const = 0;
	// Gets the top-left corner of the widget.
	glm::vec2 tl() const;

	/////////////////////////////////////////////////////////////// OPACITY ///////////////////////////////////////////////////////////////

	// Gets the opacity of the widget.
	float opacity() const;
	// Hides the widget instantly.
	void hide();
	// Hides the widget gradually.
	void hide(ticks time);
	// Unhides the widget instantly.
	void unhide();
	// Unhides the widget gradually.
	void unhide(ticks time);

	/////////////////////////////////////////////////////////////// STATUS ////////////////////////////////////////////////////////////////

	// Gets whether the widget is hoverable.
	bool hoverable() const;
	// Gets whether the widget is writable.
	bool writable() const;
	// Gets whether the widget is active.
	virtual bool active() const;

	////////////////////////////////////////////////////////// MOUSE INTERACTION //////////////////////////////////////////////////////////

	// Callback for the widget being hovered.
	virtual void on_hover() {}
	// Callback for the widget being unhovered.
	virtual void on_unhover() {}
	// Callback for the widget being clicked and starting to be held.
	virtual void on_hold_begin() {}
	// Callback for the widget being held via hold transfer.
	virtual void on_hold_transfer_in() {}
	// Callback for the widget being unheld via hold transfer.
	virtual void on_hold_transfer_out() {}
	// Callback for the widget being unheld via click release.
	virtual void on_hold_end() {}

	//////////////////////////////////////////////////////// KEYBOARD INTERACTION /////////////////////////////////////////////////////////

	// Callback for gaining input focus.
	virtual void on_gain_focus() {};
	// Callback for losing input focus.
	virtual void on_lose_focus() {};
	// Callback for character insertion.
	virtual void on_write(std::string_view){};
	// Callback for when enter is pressed.
	virtual void on_enter() {};
	// Callback for character deletion.
	virtual void on_erase() {};
	// Callback for input clearing.
	virtual void on_clear() {};
	// Callback for text copying.
	virtual void on_copy() {};
	// Callback for text pasting.
	virtual void on_paste() {};
	// Callback for one of the widget's shortcuts being pressed.
	virtual void on_shortcut() {}

	////////////////////////////////////////////////////////////// UPDATING ///////////////////////////////////////////////////////////////

	// Updates the widget.
	virtual void update();
	// Instructs the widget to release its graphical resources.
	virtual void release_graphical_resources() {}
	// Adds the widget to the renderer.
	virtual void add_to_renderer() = 0;

  private:
	// The opacity of the widget.
	interpolator<float> m_opacity;
	// Whether the widget is hoverable.
	bool m_hoverable;
	// Whether the widget is writable.
	bool m_writable;
};