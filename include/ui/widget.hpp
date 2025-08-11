#pragma once
#include "../audio.hpp"
#include "../fonts.hpp"
#include "../replay.hpp"
#include "widget_base.hpp"

/////////////////////////////////////////////////////////////// TEXT WIDGETS //////////////////////////////////////////////////////////////

// Base text widget class.
class text_widget : public widget {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a text widget.
	text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, bool hoverable, text_callback tooltip_cb,
				bool writable, font font, tr::system::ttf_style style, tr::halign text_alignment, float font_size, int max_width,
				tr::rgba8 color, text_callback text_cb);

	// Creates a common type of text widget: non-interactible, single-line.
	text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, font font, tr::system::ttf_style style,
				float font_size, text_callback text_cb, tr::rgba8 color = "A0A0A0A0"_rgba8);

	// Creates a common type of text widget: non-interactible, tooltippable, single-line.
	text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, const char* tooltip_key, font font,
				tr::system::ttf_style style, float font_size, text_callback text_cb);

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The text tint color.
	interpolator<tr::rgba8> color;
	// The callback used to get the text.
	text_callback text_cb;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Gets the size of the widget.
	glm::vec2 size() const override;
	// Updates the widget.
	void update() override;
	// Instructs the widget to release its graphical resources.
	void release_graphical_resources() override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

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
	// The alignment the text is drawn with.
	tr::halign m_text_alignment;
	// The font size used when drawing the text.
	float m_font_size;
	// The maximum allowed width of the widget's text.
	int m_max_width;
	// Cached texture and string.
	mutable std::optional<cached_t> m_cached;

	// Updates the cache.
	void update_cache() const;
};

// Clickable text widget class.
class clickable_text_widget : public text_widget {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a clickable text widget.
	clickable_text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, font font, float font_size,
						  text_callback text_cb, status_callback status_cb, action_callback action_cb,
						  text_callback tooltip_cb = NO_TOOLTIP, sound sound = sound::CONFIRM);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	void update() override;
	void add_to_renderer() override;

	bool active() const override;
	void on_hover() override;
	void on_unhover() override;
	void on_hold_begin() override;
	void on_hold_transfer_in() override;
	void on_hold_transfer_out() override;
	void on_hold_end() override;

	void on_shortcut() override;

  private:
	// Callback used to determine the status of the widget.
	status_callback m_status_cb;
	// Callback called when the widget is interacted with.
	action_callback m_action_cb;
	// Timer used when overriding the disabled color.
	ticks m_override_disabled_color_left;
	// The sound effect that interacting with the widget plays.
	sound m_sound;
};

// Widget for inputting a line of text.
template <std::size_t S> class line_input_widget : public text_widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a text line input wiget.
	line_input_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, tr::system::ttf_style style, float font_size,
					  status_callback status_cb, action_callback enter_cb, std::string_view initial_text);

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The input buffer.
	tr::static_string<S * 4> buffer;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	void add_to_renderer() override;
	bool active() const override;

	void on_hover() override;
	void on_unhover() override;
	void on_hold_begin() override;
	void on_hold_transfer_in() override;
	void on_hold_transfer_out() override;
	void on_hold_end() override;
	void on_gain_focus() override;
	void on_lose_focus() override;
	void on_write(std::string_view input) override;
	void on_enter() override;
	void on_erase() override;
	void on_clear() override;
	void on_copy() override;
	void on_paste() override;

  private:
	// Callback used to determine the status of the widget.
	status_callback m_status_cb;
	// Callback called when enter is pressed.
	action_callback m_enter_cb;
	// Keeps track of whether the widget has input focus.
	bool m_has_focus;
};

// Widget for inputting multiline text.
template <std::size_t S> class multiline_input_widget : public text_widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a multiline input wiget.
	multiline_input_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, float width, std::uint8_t max_lines,
						   tr::halign text_alignment, float font_size, status_callback status_cb);

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The input buffer.
	tr::static_string<S * 4> buffer;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	glm::vec2 size() const override;
	void add_to_renderer() override;
	bool active() const override;

	void on_hover() override;
	void on_unhover() override;
	void on_hold_begin() override;
	void on_hold_transfer_in() override;
	void on_hold_transfer_out() override;
	void on_hold_end() override;
	void on_gain_focus() override;
	void on_lose_focus() override;
	void on_write(std::string_view input) override;
	void on_enter() override;
	void on_erase() override;
	void on_clear() override;
	void on_copy() override;
	void on_paste() override;

  private:
	// Callback used to determine the status of the widget.
	status_callback m_status_cb;
	// The size of the widget.
	glm::vec2 m_size;
	// The maximum allowed number of lines of text.
	std::uint8_t m_max_lines;
	// Keeps track of whether the widget has input focus.
	bool m_has_focus;
};

///////////////////////////////////////////////////////////// NON-TEXT WIDGETS ////////////////////////////////////////////////////////////

// Image widget.
class image_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates an image widget.
	image_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, int priority, std::string_view file,
				 std::uint16_t* hue_ref = nullptr);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Gets the size of the widget.
	glm::vec2 size() const override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

	// No need to specialize release_graphical_resources because this class of widget is not used in the settings menu.

  private:
	// The image texture.
	tr::gfx::texture m_texture;
	// A reference to the hue to apply to the widget, or nullptr to not tint the widget.
	std::uint16_t* m_hue_ref;
	// The drawing priority of the image.
	int m_priority;
};

// Color preview widget.
class color_preview_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a color preview widget.
	color_preview_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, std::uint16_t& hue_ref);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	glm::vec2 size() const override;
	void add_to_renderer() override;

  private:
	// Reference to the hue value.
	std::uint16_t& m_hue_ref;
};

// Clickable arrow widget.
class arrow_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates an arrow widget.
	arrow_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, bool right_arrow, status_callback status_cb,
				 action_callback action_cb);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	glm::vec2 size() const override;
	void add_to_renderer() override;
	void update() override;

	bool active() const override;
	void on_hover() override;
	void on_unhover() override;
	void on_hold_begin() override;
	void on_hold_transfer_in() override;
	void on_hold_transfer_out() override;
	void on_hold_end() override;

	void on_shortcut() override;

  protected:
	// Whether this widget is a right arrow.
	bool m_right;
	// The tint color.
	interpolator<tr::rgba8> m_color;
	// Callback used to determine the status of the widget.
	status_callback m_status_cb;
	// Callback called when the widget is interacted with.
	action_callback m_action_cb;
	// Timer used when overriding the disabled color.
	ticks m_override_disabled_color_left;
};

// Replay playback indicator widget.
struct replay_playback_indicator_widget : public widget {
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a replay playback indicator widget.
	replay_playback_indicator_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	glm::vec2 size() const override;
	void add_to_renderer() override;
};

////////////////////////////////////////////////////////////// MIXED WIDGETS //////////////////////////////////////////////////////////////

// Score widget.
struct score_widget : public text_widget {
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	// Sentinel to not display the rank of the widget.
	static constexpr std::size_t DONT_SHOW_RANK{std::numeric_limits<std::size_t>::max()};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a score widget.
	score_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, std::size_t rank, score* score);

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The rank of the score.
	std::size_t rank;
	// Pointer to the score this widget is displaying information for.
	score* score;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	glm::vec2 size() const override;
	void add_to_renderer() override;
};

// Replay widget.
struct replay_widget : public clickable_text_widget {
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	replay_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, auto base_status_cb, auto base_action_cb,
				  std::optional<std::map<std::string, replay_header>::iterator> it);

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The replay header.
	std::optional<std::map<std::string, replay_header>::iterator> it;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	glm::vec2 size() const override;
	void add_to_renderer() override;
};

#include "widget_impl.hpp"