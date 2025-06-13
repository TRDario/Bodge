#pragma once
#include "../font_manager.hpp"
#include "../replay.hpp"
#include "widget_base.hpp"

/////////////////////////////////////////////////////////////// TEXT WIDGETS //////////////////////////////////////////////////////////////

// Base text widget class.
class text_widget : public widget {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a text widget.
	text_widget(string_view name, vec2 pos, align alignment, bool hoverable, tooltip_callback tooltip_cb, bool writable,
				vector<key_chord>&& shortcuts, font font, ttf_style style, halign text_alignment, float font_size, int max_width,
				rgba8 color, text_callback text_cb);

	// Creates a common type of text widget: non-interactible, single-line.
	text_widget(string_view name, vec2 pos, align alignment, font font, ttf_style style, float font_size,
				text_callback text_cb = DEFAULT_TEXT_CALLBACK, rgba8 color = {160, 160, 160, 160});

	// Creates a common type of text widget: non-interactible, tooltippable, single-line.
	text_widget(string_view name, vec2 pos, align alignment, string_view tooltip_key, font font, ttf_style style, float font_size,
				text_callback text_cb = DEFAULT_TEXT_CALLBACK);

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The text tint color.
	interpolated_rgba8 color;
	// The callback used to get the text.
	text_callback text_cb;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Gets the size of the widget.
	vec2 size() const override;
	// Updates the widget.
	void update() noexcept override;
	// Instructs the widget to release its graphical resources.
	void release_graphical_resources() noexcept override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

  protected:
	struct cached {
		// The texture of the text.
		texture texture;
		// The amount of texture that's actually being used.
		vec2 size;
		// The text in the texture.
		string text;
	};

	// The font used when drawing the text.
	font _font;
	// The style used when drawing the font.
	ttf_style _style;
	// The alignment the text is drawn with.
	halign _text_alignment;
	// The font size used when drawing the text.
	float _font_size;
	// The maximum allowed width of the widget's text.
	int _max_width;
	// Cached texture and string.
	mutable optional<cached> _cached;

	// Updates the cache.
	void update_cache() const;
};

// Clickable text widget class.
class clickable_text_widget : public text_widget {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a clickable text widget.
	clickable_text_widget(string_view name, vec2 pos, align alignment, font font, float font_size, text_callback text_cb,
						  status_callback status_cb, action_callback action_cb, tooltip_callback tooltip_cb = NO_TOOLTIP,
						  vector<key_chord>&& shortcuts = {}) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	void add_to_renderer() override;

	bool active() const noexcept override;
	void on_hover() noexcept override;
	void on_unhover() noexcept override;
	void on_hold_begin() noexcept override;
	void on_hold_transfer_in() noexcept override;
	void on_hold_transfer_out() noexcept override;
	void on_hold_end() noexcept override;

	void on_shortcut() noexcept override;

  private:
	// Callback used to determine the status of the widget.
	status_callback _status_cb;
	// Callback called when the widget is interacted with.
	action_callback _action_cb;
};

// Widget for inputting a line of text.
template <size_t S> class line_input_widget : public text_widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a text line input wiget.
	line_input_widget(string_view name, vec2 pos, align alignment, float font_size, status_callback status_cb,
					  action_callback enter_cb) noexcept;

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The input buffer.
	static_string<S> buffer;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	void add_to_renderer() override;
	bool active() const noexcept override;

	void on_hover() noexcept override;
	void on_unhover() noexcept override;
	void on_hold_begin() noexcept override;
	void on_hold_transfer_in() noexcept override;
	void on_hold_transfer_out() noexcept override;
	void on_hold_end() noexcept override;
	void on_gain_focus() noexcept override;
	void on_lose_focus() noexcept override;
	void on_write(string_view input) noexcept override;
	void on_enter() override;
	void on_erase() noexcept override;
	void on_clear() noexcept override;
	void on_copy() noexcept override;
	void on_paste() noexcept override;

  private:
	// Callback used to determine the status of the widget.
	status_callback _status_cb;
	// Callback called when enter is pressed.
	action_callback _enter_cb;
	// Keeps track of whether the widget has input focus.
	bool _has_focus;
};

// Widget for inputting multiline text.
template <size_t S> class multiline_input_widget : public text_widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a multiline input wiget.
	multiline_input_widget(string_view name, vec2 pos, align alignment, float width, u8 max_lines, halign text_alignment, float font_size,
						   status_callback status_cb) noexcept;

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The input buffer.
	static_string<S> buffer;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const override;
	void add_to_renderer() override;
	bool active() const noexcept override;

	void on_hover() noexcept override;
	void on_unhover() noexcept override;
	void on_hold_begin() noexcept override;
	void on_hold_transfer_in() noexcept override;
	void on_hold_transfer_out() noexcept override;
	void on_hold_end() noexcept override;
	void on_gain_focus() noexcept override;
	void on_lose_focus() noexcept override;
	void on_write(string_view input) noexcept override;
	void on_enter() override;
	void on_erase() noexcept override;
	void on_clear() noexcept override;
	void on_copy() noexcept override;
	void on_paste() noexcept override;

  private:
	// Callback used to determine the status of the widget.
	status_callback _status_cb;
	// The size of the widget.
	glm::vec2 _size;
	// The maximum allowed number of lines of text.
	u8 _max_lines;
	// Keeps track of whether the widget has input focus.
	bool _has_focus;
};

///////////////////////////////////////////////////////////// NON-TEXT WIDGETS ////////////////////////////////////////////////////////////

// Color preview widget.
class color_preview_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a color preview widget.
	color_preview_widget(string_view name, vec2 pos, align alignment, u16& hue_ref) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	void add_to_renderer() override;

  private:
	// Reference to the hue value.
	u16& _hue_ref;
};

// Clickable arrow widget.
class arrow_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates an arrow widget.
	arrow_widget(string_view name, vec2 pos, align alignment, bool right_arrow, status_callback status_cb, action_callback action_cb,
				 vector<key_chord>&& chords = {}) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	void add_to_renderer() override;
	void update() noexcept override;

	bool active() const noexcept override;
	void on_hover() noexcept override;
	void on_unhover() noexcept override;
	void on_hold_begin() noexcept override;
	void on_hold_transfer_in() noexcept override;
	void on_hold_transfer_out() noexcept override;
	void on_hold_end() noexcept override;

	void on_shortcut() noexcept override;

  protected:
	// Whether this widget is a right arrow.
	bool _right;
	// The tint color.
	interpolated_rgba8 _color;
	// Callback used to determine the status of the widget.
	status_callback _status_cb;
	// Callback called when the widget is interacted with.
	action_callback _action_cb;
};

// Replay playback indicator widget.
struct replay_playback_indicator_widget : public widget {
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a replay playback indicator widget.
	replay_playback_indicator_widget(string_view name, vec2 pos, align alignment) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	void add_to_renderer() override;
};

////////////////////////////////////////////////////////////// MIXED WIDGETS //////////////////////////////////////////////////////////////

// Score widget.
struct score_widget : public text_widget {
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a score widget.
	score_widget(string_view name, vec2 pos, align alignment, size_t rank, score* score) noexcept;

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The rank of the score.
	size_t rank;
	// Pointer to the score this widget is displaying information for.
	score* score;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	vec2 tl() const noexcept override;
	void add_to_renderer() override;
};

// Replay widget.
struct replay_widget : public clickable_text_widget {
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	replay_widget(string_view name, vec2 pos, align alignment, auto base_status_cb, auto base_action_cb,
				  optional<map<string, replay_header>::iterator> it, key shortcut);

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The replay header.
	optional<map<string, replay_header>::iterator> it;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	vec2 tl() const noexcept override;
	void add_to_renderer() override;
};

#include "widget_impl.hpp"