#pragma once
#include "../audio.hpp"
#include "../replay.hpp"
#include "../text_engine.hpp"
#include "../timer.hpp"
#include "widget_base.hpp"

/////////////////////////////////////////////////////////// FORWARD DECLARATIONS //////////////////////////////////////////////////////////

class ui_manager;
class replays_state;

/////////////////////////////////////////////////////////////// LABEL WIDGET //////////////////////////////////////////////////////////////

// Static text label widget.
struct label_widget : public text_widget {
	// Creates a label widget.
	label_widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb,
				 tr::sys::ttf_style style, float font_size, tr::rgba8 color = GRAY);

	// The tint of the widget.
	tweened_color tint;

	void tick() override;
	void add_to_renderer() override;
};

//////////////////////////////////////////////////////////// TEXT BUTTON WIDGET ///////////////////////////////////////////////////////////

// Text widget that can be interacted with as a button.
class text_button_widget : public text_widget {
  public:
	// Creates a text button widget.
	text_button_widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb,
					   font font, float font_size, status_callback status_cb, action_callback action_cb, sound action_sound);

	void tick() override;
	void add_to_renderer() override;

	bool interactible() const override;
	void on_action() override;
	void on_hover() override;
	void on_unhover() override;
	void on_held() override;
	void on_unheld() override;
	void on_selected() override;
	void on_unselected() override;

  private:
	// Callback used to determine whether the button is interactible.
	status_callback m_scb;
	// Action executed when the button is interacted with.
	action_callback m_acb;
	// The sound played when the button is interacted with.
	sound m_action_sound;

  protected:
	// The tint of the button.
	tweened_color m_tint;

  private:
	// Flag denoting whether the button is currently hovered over.
	bool m_hovered;
	// Flag denoting whether the button is currently held.
	bool m_held;
	// Flag denoting whether the button is currently selected.
	bool m_selected;
	// Timer used for a post-action animation that counts down after an action.
	decrementing_timer<0.36_s> m_action_animation_timer;
};

/////////////////////////////////////////////////////////// NUMERIC INPUT WIDGET //////////////////////////////////////////////////////////

// Must be initialized before text_widget, so is separated out into its own struct.
template <class T, usize S> struct basic_numeric_input_widget_data {
	// Reference to the UI manager.
	ui_manager& m_ui;
	// Reference to the variable the input is for.
	T& m_ref;
	// Buffer to write into.
	tr::static_string<S> m_buffer;
	// Callback used to determine whether the input is interactible.
	status_callback m_scb;
	// Callback used to validate the value after input is finished.
	validation_callback<T> m_vcb;
	// The tint of the input.
	tweened_color m_tint;
	// Flag denoting whether the input is currently hovered over.
	bool m_hovered;
	// Flag denoting whether the input is currently held.
	bool m_held;
	// Flag denoting whether the input is currently selected.
	bool m_selected;
};

// Widget used to input a numeric value.
// Formatter is a class with static members from_string() (string -> value) and to_string() (value/string_view -> string).
template <class T, usize Digits, class Formatter>
struct basic_numeric_input_widget : private basic_numeric_input_widget_data<T, Digits>, public text_widget {
	// Creates a numeric input widget.
	basic_numeric_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, float font_size, ui_manager& ui, T& ref,
							   status_callback status_cb, validation_callback<T> validation_cb);

	void add_to_renderer() override;
	void tick() override;
	bool interactible() const override;
	bool writable() const override;

	void on_action() override;
	void on_hover() override;
	void on_unhover() override;
	void on_held() override;
	void on_unheld() override;
	void on_selected() override;
	void on_unselected() override;
	void on_write(std::string_view input) override;
	void on_enter() override;
	void on_erase() override;
	void on_clear() override;
};

// Default numeric input widget formatter (uses std::from_chars and std::format).
template <class T, tr::template_string_literal Format, tr::template_string_literal BufferFormat> struct default_numeric_input_formatter {
	static void from_string(std::common_type_t<T, int>& out, std::string_view str);
	static std::string to_string(T v);
	static std::string to_string(std::string_view str);
};
// Input widget for most numeric variables.
template <class T, usize Digits, tr::template_string_literal Format = "{}", tr::template_string_literal BufferFormat = "{}">
using numeric_input_widget = basic_numeric_input_widget<T, Digits, default_numeric_input_formatter<T, Format, BufferFormat>>;

// Special interval formatter for a numeric input widget.
struct interval_formatter {
	static void from_string(ticks& out, std::string_view str);
	static std::string to_string(ticks v);
	static std::string to_string(std::string_view str);
};
// Input widget for interval variables.
template <usize Digits> using interval_input_widget = basic_numeric_input_widget<ticks, Digits, interval_formatter>;

//////////////////////////////////////////////////////////// LINE INPUT WIDGET ////////////////////////////////////////////////////////////

// Must be initialized before text_widget, so is separated out into its own struct.
template <usize S> struct input_buffer {
	// Text input buffer.
	tr::static_string<S * 4> buffer;
};

// Widget used to input a line of text.
template <usize S> class line_input_widget : public input_buffer<S>, public text_widget {
  public:
	// Creates a line input widget.
	line_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, tr::sys::ttf_style style, float font_size,
					  status_callback status_cb, action_callback enter_cb, std::string_view initial_text = {});

	void add_to_renderer() override;
	void tick() override;
	bool interactible() const override;
	bool writable() const override;

	void on_action() override;
	void on_hover() override;
	void on_unhover() override;
	void on_held() override;
	void on_unheld() override;
	void on_selected() override;
	void on_unselected() override;
	void on_write(std::string_view input) override;
	void on_enter() override;
	void on_erase() override;
	void on_clear() override;
	void on_copy() override;
	void on_paste() override;

  private:
	// Callback used to determine whether the input is interactible.
	status_callback m_scb;
	// Function called when enter is pressed while the widget is selected.
	action_callback m_enter_cb;
	// The tint of the input.
	tweened_color m_tint;
	// Flag denoting whether the input is currently hovered over.
	bool m_hovered;
	// Flag denoting whether the input is currently held.
	bool m_held;
	// Flag denoting whether the input is currently selected.
	bool m_selected;
};

////////////////////////////////////////////////////////// MULTILINE INPUT WIDGET /////////////////////////////////////////////////////////

// Widget used to input multiple lines of text.
template <usize S> class multiline_input_widget : public input_buffer<S>, public text_widget {
  public:
	// Creates a multiline input widget.
	multiline_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, float width, u8 max_lines, float font_size,
						   status_callback status_cb);

	glm::vec2 size() const override;
	void add_to_renderer() override;
	void tick() override;
	bool interactible() const override;
	bool writable() const override;

	void on_action() override;
	void on_hover() override;
	void on_unhover() override;
	void on_held() override;
	void on_unheld() override;
	void on_selected() override;
	void on_unselected() override;
	void on_write(std::string_view input) override;
	void on_enter() override;
	void on_erase() override;
	void on_clear() override;
	void on_copy() override;
	void on_paste() override;

  private:
	// Callback used to determine whether the input is interactible.
	status_callback m_scb;
	// Size of the input box.
	glm::vec2 m_size;
	// Maximum allowed number of lines in the input.
	u8 m_max_lines;
	// The tint of the input.
	tweened_color m_tint;
	// Flag denoting whether the input is currently hovered over.
	bool m_hovered;
	// Flag denoting whether the input is currently held.
	bool m_held;
	// Flag denoting whether the input is currently selected.
	bool m_selected;
};

/////////////////////////////////////////////////////////////// IMAGE WIDGET //////////////////////////////////////////////////////////////

// Widget used to display an image.
class image_widget : public widget {
  public:
	// Creates an image widget.
	image_widget(tweened_position pos, tr::align alignment, ticks unhide_time, int priority, std::string_view file,
				 tr::opt_ref<u16> hue = std::nullopt);

	glm::vec2 size() const override;
	void add_to_renderer() override;

  private:
	// The image texture, don't immediately load into a GPU texture to ensure it can be asynchronously loaded.
	std::variant<tr::bitmap, tr::gfx::texture> m_texture;
	// Optional reference to a hue the image takes on.
	tr::opt_ref<u16> m_hue;
	// The drawing priority of the widget.
	int m_priority;
};

/////////////////////////////////////////////////////////// COLOR PREVIEW WIDGET //////////////////////////////////////////////////////////

// Widget used to display a color previews.
class color_preview_widget : public widget {
  public:
	// Creates a color preview widget.
	color_preview_widget(tweened_position pos, tr::align alignment, ticks unhide_time, u16& hue);

	glm::vec2 size() const override;
	void add_to_renderer() override;

  private:
	// Reference to the hue of the color to display.
	u16& m_hue;
};

/////////////////////////////////////////////////////////////// ARROW WIDGET //////////////////////////////////////////////////////////////

// Arrow orientation.
enum class arrow_type : bool {
	LEFT,
	RIGHT
};

// Arrow widget used to change values or select an option.
class arrow_widget : public widget {
  public:
	// Creates an arrow widget.
	arrow_widget(tweened_position pos, tr::valign alignment, ticks unhide_time, arrow_type type, status_callback status_cb,
				 action_callback action_cb);

	glm::vec2 size() const override;
	void add_to_renderer() override;
	void tick() override;

	bool interactible() const override;
	void on_action() override;
	void on_hover() override;
	void on_unhover() override;
	void on_held() override;
	void on_unheld() override;
	void on_selected() override;
	void on_unselected() override;

  protected:
	// Callback used to determine whether the arrow is interactible.
	status_callback m_scb;
	// Action executed when the arrow is interacted with.
	action_callback m_acb;
	// The tint of the arrow.
	tweened_color m_tint;
	// The orientation of the arrow.
	arrow_type m_type;
	// Flag denoting whether the arrow is currently hovered over.
	bool m_hovered;
	// Flag denoting whether the arrow is currently held.
	bool m_held;
	// Flag denoting whether the arrow is currently selected.
	bool m_selected;
	// Timer used for a post-action animation that counts down after an action.
	decrementing_timer<0.36_s> m_action_animation_timer;
};

/////////////////////////////////////////////////////////////// ARROW WIDGET //////////////////////////////////////////////////////////////

// Widget used to display the replay playback speed.
struct replay_playback_indicator_widget : public widget {
	// Creates a replay playback indicator widget.
	replay_playback_indicator_widget(tweened_position pos, tr::align alignment, ticks unhide_time);

	glm::vec2 size() const override;
	void add_to_renderer() override;
};

/////////////////////////////////////////////////////////////// SCORE WIDGET //////////////////////////////////////////////////////////////

// Widget used to display a time or score result.
struct score_widget : public text_widget {
	// Score widget types.
	enum class type {
		// Showing a time.
		TIME,
		// Showing a score.
		SCORE = 4
	};

	// Creates a score widget.
	score_widget(tweened_position pos, tr::align alignment, ticks unhide_time, enum type type, usize rank,
				 tr::opt_ref<const score_entry> score);

	glm::vec2 size() const override;
	void add_to_renderer() override;

  private:
	// Flag denoting whether the widget is for an empty score slot.
	bool m_empty;
	// Additional flags of the score the widget represents.
	score_flags m_flags;
};

////////////////////////////////////////////////////////////// REPLAY WIDGET //////////////////////////////////////////////////////////////

// Must be initialized before text_button_widget, so is separated out into its own struct.
struct replay_widget_data {
	// Reference to the parent replays state.
	replays_state& m_parent_state;
	// Iterator to the entry in the replay map corresponding to this widget.
	std::optional<replay_map::const_iterator> m_replay_it;
};

// Button widget used to display replay information and select a replay to be played.
struct replay_widget : private replay_widget_data, public text_button_widget {
	// Creates a replay widget.
	replay_widget(tweened_position pos, tr::align alignment, ticks unhide_time, replays_state& state,
				  std::optional<replay_map::const_iterator> replay_it);

	glm::vec2 size() const override;
	void add_to_renderer() override;
};

////////////////////////////////////////////////////////////// IMPLEMENTATION /////////////////////////////////////////////////////////////

#include "line_input_widget_impl.hpp"      // IWYU pragma: keep
#include "multiline_input_widget_impl.hpp" // IWYU pragma: keep
#include "numeric_input_widget_impl.hpp"   // IWYU pragma: keep