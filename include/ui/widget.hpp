#pragma once
#include "../font_manager.hpp"
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
	// Instructs the widget to release its graphical resources.
	virtual void release_graphical_resources() noexcept;
	// Adds the widget to the renderer.
	virtual void add_to_renderer() = 0;

  private:
	// The opacity of the widget.
	interpolated_float _opacity;
};

// Alias for the type of a tooltip callback.
using tooltip_callback = std::function<std::string()>;
// Sentinel for a mousable to not have a tooltip.
inline const tooltip_callback NO_TOOLTIP{};

// Interface for a mousable widget.
struct mousable {
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a mousable.
	mousable(tooltip_callback tooltip_cb = NO_TOOLTIP) noexcept;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// The tooptip callback (optional).
	tooltip_callback tooltip_cb;

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

//////////////////////////////////////////////////////// GRAPHICAL WIDGET CLASSES /////////////////////////////////////////////////////////

// Alias for a text widget text callback.
using text_callback = std::function<std::string(const std::string&)>;
// Default callback for text widgets: gettnig a localization string using the widget name as the key.
inline const text_callback DEFAULT_TEXT_CALLBACK{[](const std::string& name) { return std::string{localization[name]}; }};

// Widget used to draw basic, non-interactable text.
class basic_text_widget : public widget {
  public:
	// Creates a basic text widget.
	// The callback is a function object that takes in the name of the object and returns a string.
	basic_text_widget(std::string&& name, glm::vec2 pos, tr::align alignment, tr::rgba8 color, font font, float font_size, float outline,
					  float max_width = tr::UNLIMITED_WIDTH, tr::halign text_alignment = tr::halign::CENTER,
					  text_callback text_cb = DEFAULT_TEXT_CALLBACK) noexcept;
	// Shorthand constructor for a common kind of basic text widget.
	basic_text_widget(std::string&& name, glm::vec2 pos, tr::align alignment, float font_size) noexcept;

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The text tint color.
	interpolated_rgba8 color;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Gets the size of the widget.
	glm::vec2 size() const noexcept override;
	// Updates the widget.
	void update() noexcept override;
	// Instructs the widget to release its graphical resources.
	void release_graphical_resources() noexcept override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

  private:
	struct cached {
		// The texture of the text.
		tr::texture texture;
		// The amount of texture that's actually being used.
		glm::vec2 size;
		// The text in the texture.
		std::string text;
	};

	// The font used when drawing the text.
	font _font;
	// The font size used when drawing the text.
	float _font_size;
	// The outline thickness used when drawing the text.
	float _outline;
	// The maximum allowed width used when drawing the text.
	float _max_width;
	// Alignment used when drawing the text.
	tr::halign _text_alignment;
	// The callback used to get the text.
	text_callback _text_cb;
	// Cached texture and string.
	std::optional<cached> _cached;

	// Updates the cache.
	void update_cache();
};

class tooltippable_text_widget : public basic_text_widget, public mousable {
  public:
	// Creates a tooltippable text widget.
	tooltippable_text_widget(std::string&& name, glm::vec2 pos, tr::align alignment, std::string_view tooltip_key,
							 float font_size) noexcept;

	//

	bool holdable() const noexcept override;
	void on_hover() noexcept override {}
	void on_unhover() noexcept override {}
	void on_hold_begin() noexcept override {};
	void on_hold_transfer_in() noexcept override {};
	void on_hold_transfer_out() noexcept override {};
	void on_hold_end() noexcept override {};
};
