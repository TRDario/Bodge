#pragma once
#include "../localization.hpp"
#include "interpolated.hpp"

////////////////////////////////////////////////////////////////// TYPES //////////////////////////////////////////////////////////////////

// Shortcut key chord.
struct key_chord {
	// Shortcut chord key.
	tr::keycode key;
	// Shortcut chord modifiers.
	tr::keymods mods{tr::keymods::NONE};

	bool operator==(const key_chord&) const = default;
};

// Alias for the type of a tooltip callback.
using tooltip_callback = std::function<std::string()>;
// Sentinel for a mousable to not have a tooltip.
inline const tooltip_callback NO_TOOLTIP{};

// Alias for a text widget text callback.
using text_callback = std::function<std::string(const tr::static_string<30>&)>;
// Default callback for text widgets: gettnig a localization std::string using the widget name as the key.
inline const text_callback DEFAULT_TEXT_CALLBACK{[](const tr::static_string<30>& name) { return std::string{localization[name]}; }};

// Alias for a clickable widget status callback.
using status_callback = std::function<bool()>;
// Alias for a clickable widget action callback.
using action_callback = std::function<void()>;

// Sentinel for no shortcuts.
constexpr std::initializer_list<key_chord> NO_SHORTCUTS{};

////////////////////////////////////////////////////////////////// WIDGET /////////////////////////////////////////////////////////////////

// Base widget class.
class widget {
  public:
	// Creates a widget.
	widget(std::string_view name, glm::vec2 pos, tr::align alignment, bool hoverable, tooltip_callback tooltip_cb, bool writable,
		   std::vector<key_chord>&& shortcuts) noexcept;
	// Virtual destructor.
	virtual ~widget() noexcept = default;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// The name of the widget.
	tr::static_string<30> name;
	// The alignment of the widget.
	tr::align alignment;
	// The position of the widget.
	interpolated_vec2 pos;
	// An optional tooltip callback (the widget must be hoverable for it to be used).
	tooltip_callback tooltip_cb;

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
	void hide(ticks time) noexcept;
	// Unhides the widget instantly.
	void unhide() noexcept;
	// Unhides the widget gradually.
	void unhide(ticks time) noexcept;

	/////////////////////////////////////////////////////////////// STATUS ////////////////////////////////////////////////////////////////

	// Gets whether the widget is hoverable.
	bool hoverable() const noexcept;
	// Gets whether the widget is writable.
	bool writable() const noexcept;
	// Gets whether the widget is active.
	virtual bool active() const noexcept;

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

	//////////////////////////////////////////////////////// SHORTCUT INTERACTION /////////////////////////////////////////////////////////

	// Gets whether a chord is a valid shortcut for this widget.
	bool is_shortcut(const key_chord& chord) const noexcept;
	// Callback for one of the widget's shortcuts being pressed.
	virtual void on_shortcut() {}

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

	////////////////////////////////////////////////////////////// UPDATING ///////////////////////////////////////////////////////////////

	// Updates the widget.
	virtual void update();
	// Instructs the widget to release its graphical resources.
	virtual void release_graphical_resources() noexcept {}
	// Adds the widget to the renderer.
	virtual void add_to_renderer() = 0;

  private:
	// The opacity of the widget.
	interpolated_float _opacity;
	// Whether the widget is hoverable.
	bool _hoverable;
	// Whether the widget is writable.
	bool _writable;
	// The shortcuts of the widget.
	std::vector<key_chord> _shortcuts;
};