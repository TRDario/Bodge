#pragma once
#include "../fonts.hpp"
#include "interpolator.hpp"

////////////////////////////////////////////////////////////////// TYPES //////////////////////////////////////////////////////////////////

// Alias for a widget text callback.
using text_callback = std::function<std::string()>;
// Sentinel for a mousable to not have a tooltip.
inline const text_callback NO_TOOLTIP{};
// Common callback for text widgets: getting a localization string using the widget name as the key.
struct loc_text_callback {
	tag tag;

	std::string operator()() const;
};
// Common callback for text widgets: getting a tooltip localization string.
struct tooltip_loc_text_callback {
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
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a widget.
	widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, bool writable);
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

	// Gets whether the widget is hidden.
	bool hidden() const;
	// Gets whether the widget is interactible.
	virtual bool interactible() const;
	// Gets whether the widget is writable.
	bool writable() const;

	///////////////////////////////////////////////////////////// INTERACTION /////////////////////////////////////////////////////////////

	// Callback for the widget being interacted with.
	virtual void on_action() {}
	// Callback for the widget being hovered.
	virtual void on_hover() {}
	// Callback for the widget being unhovered.
	virtual void on_unhover() {}
	// Callback for the widget being held.
	virtual void on_held() {}
	// Callback for the widget being unheld.
	virtual void on_unheld() {}
	// Callback for the widget being selected via keyboard.
	virtual void on_selected() {};
	// Callback for the widget being unselected.
	virtual void on_unselected() {};
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
	virtual void release_graphical_resources() {}
	// Adds the widget to the renderer.
	virtual void add_to_renderer() = 0;

  private:
	// The opacity of the widget.
	interpolator<float> m_opacity;
	// Whether the widget is writable.
	const bool m_writable;
};

// Base text widget class.
class text_widget : public widget {
  public:
	// Creates a text widget.
	text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, bool writable,
				text_callback text_cb, font font, tr::system::ttf_style style, float font_size, int max_width);

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The callback used when getting the text of the widget.
	text_callback text_cb;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Gets the size of the widget.
	glm::vec2 size() const override;
	// Instructs the widget to release its graphical resources.
	void release_graphical_resources() override;

  protected:
	struct cached_t {
		// The texture of the text.
		tr::gfx::texture texture;
		// The amount of texture that's actually being used.
		glm::vec2 size;
		// The text in the texture.
		std::string text;
	};

	// The font used when drawing the text.
	font m_font;
	// The style used when drawing the font.
	tr::system::ttf_style m_style;
	// The font size used when drawing the text.
	float m_font_size;
	// The maximum allowed width of the widget's text.
	int m_max_width;
	// Cached texture and string.
	mutable std::optional<cached_t> m_cached;

	// Updates the cache.
	void update_cache() const;
	// Adds the widget to the renderer.
	void add_to_renderer_raw(tr::rgba8 tint);
};