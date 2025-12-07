#pragma once
#include "../fonts.hpp"
#include "tweening.hpp"

///////////////////////////////////////////////////////////////// COMMON //////////////////////////////////////////////////////////////////

// Widget tag.
using tag = const char*;
// Information about a label widget.
struct label_info {
	// Tag of the label widget.
	tag tag;
	// Localization key of the label's tooltip.
	const char* tooltip;
};

// List of widget tags considered to be on the same horizontal line for keyboard navigation purposes.
using selection_tree_row = std::initializer_list<tag>;
// List of rows of widget tags for keyboard navigation purposes.
using selection_tree = std::initializer_list<selection_tree_row>;
// Table mapping key chords to widget tags.
using shortcut_table = std::initializer_list<std::pair<const tr::sys::key_chord, tag>>;

// Function returning a string used by a text widget or for a tooltip.
using text_callback = std::function<std::string()>;
// Function returning a boolean denoting whether a widget is considered interactible.
using status_callback = std::function<bool()>;
// Function used for a widget action.
using action_callback = std::function<void()>;
// Function used for validating a widget value.
template <class T> using validation_callback = std::function<T(std::common_type_t<T, int>)>;

// Special text callback value used to denote that a widget has no tooltip.
inline const text_callback NO_TOOLTIP{};
// Common callback for text widgets: getting a localization string using the widget name as the key.
struct loc_text_callback {
	// The tag serving as the key for the localization lookup.
	tag tag;

	std::string operator()() const;
};
// Common callback for text widgets: getting a tooltip localization string.
struct tooltip_loc_text_callback {
	// The tag serving as the key for the localization lookup.
	tag tag;

	std::string operator()() const;
};
// Common callback for text widgets: copying a string directly.
struct const_text_callback {
	// The string to copy.
	std::string str;

	std::string operator()() const;
};
// Common callback for text widgets: copying from a buffer if non-empty or a localized "empty" otherwise.
template <usize S> struct buffer_text_callback {
	// Reference to the text buffer.
	tr::static_string<S>& buffer;

	std::string operator()() const;
};

// Sentinel denoting that a widget should not be unhidden
constexpr ticks DONT_UNHIDE{std::numeric_limits<ticks>::max()};

////////////////////////////////////////////////////////////////// WIDGET /////////////////////////////////////////////////////////////////

// Base widget class.
class widget {
  public:
	// Creates a widget.
	widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb);
	virtual ~widget() = default;

	// The position of the widget.
	tweened_position pos;
	// Callback that returns tooltip strings.
	text_callback tooltip_cb;

	// The size of the widget.
	virtual glm::vec2 size() const = 0;
	// The top-left corner of the widget.
	glm::vec2 tl() const;

	// Gets the opacity of the widget.
	float opacity() const;
	// Hides the widget immediately.
	void hide();
	// Hides the widget gradually.
	void hide(ticks time);
	// Moves the widget while hiding it.
	void move_and_hide(glm::vec2 end, ticks time);
	// Moves the widget horizontally while hiding it.
	void move_x_and_hide(float end, ticks time);
	// Moves the widget vertically while hiding it.
	void move_y_and_hide(float end, ticks time);
	// Unhides the widget immediately.
	void unhide();
	// Unhides the widget gradually.
	void unhide(ticks time);
	// Moves the widget horizontally while unhiding it.
	void move_x_and_unhide(float end, ticks time);
	// Moves the widget vertically while unhiding it.
	void move_y_and_unhide(float end, ticks time);

	// Gets whether the widget is hidden.
	bool hidden() const;
	// Gets whether the widget is interactible.
	virtual bool interactible() const;
	// Gets whether the widget is writable.
	virtual bool writable() const;

	// Function executed when the widget is clicked or activated with enter.
	virtual void on_action() {}
	// Function executed when the widget is hovered.
	virtual void on_hover() {}
	// Function executed when the widget is unhovered.
	virtual void on_unhover() {}
	// Function executed when the widget is held.
	virtual void on_held() {}
	// Function executed when the widget is unheld.
	virtual void on_unheld() {}
	// Function executed when the widget is selected.
	virtual void on_selected() {};
	// Function executed when the widget is unselected.
	virtual void on_unselected() {};
	// Function executed when writing to a writable widget.
	virtual void on_write(std::string_view){};
	// Function executed when pressing enter on a writable widget.
	virtual void on_enter() {};
	// Function executed when erasing from a writable widget.
	virtual void on_erase() {};
	// Function executed when clearing a writable widget.
	virtual void on_clear() {};
	// Function executed when copying from a writable widget.
	virtual void on_copy() {};
	// Function executed when pasting to a writable widget.
	virtual void on_paste() {};

	// Updates the widget.
	virtual void tick();
	// Releases the widget's graphical resources.
	virtual void release_graphical_resources() {}
	// Adds the widget to the renderer.
	virtual void add_to_renderer() = 0;

  private:
	// The alignment of the widget.
	tr::align m_alignment;
	// The opacity of the widget.
	tweened_opacity m_opacity;
};

/////////////////////////////////////////////////////////////// TEXT WIDGET ///////////////////////////////////////////////////////////////

// Base text widget class.
class text_widget : public widget {
  public:
	// Creates a text widget.
	text_widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb, font font,
				tr::sys::ttf_style style, float font_size, int max_width);

	glm::vec2 size() const override;
	void release_graphical_resources() override;

  protected:
	// The font used to draw the text.
	font m_font;
	// The style used to draw the text.
	tr::sys::ttf_style m_style;
	// The size of the text font.
	float m_font_size;
	// The maximum allowed width of the widget.
	int m_max_width;
	// The text callback of the widget.
	text_callback m_text_cb;
	// The last drawn string.
	mutable std::string m_last_text;
	// Cached resources.
	mutable std::variant<std::monostate, tr::bitmap, tr::gfx::texture> m_cache;
	// The size of the last drawn string.
	mutable glm::vec2 m_last_size;

	// Updates the text cache.
	void update_cache() const;
	// Adds the widget to the renderer (must be further specialized by descendant classes).
	void add_to_renderer_raw(tr::rgba8 tint);
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <usize S> std::string buffer_text_callback<S>::operator()() const
{
	return buffer.empty() ? std::string{g_loc["empty"]} : std::string{buffer};
}