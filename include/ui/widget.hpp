///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides concrete UI widget classes.                                                                                                  //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../audio.hpp"
#include "../replay.hpp"
#include "../text_engine.hpp"
#include "../timer.hpp"
#include "widget_base.hpp"

class ui_manager;
class replays_state;

/////////////////////////////////////////////////////////////// LABEL WIDGET //////////////////////////////////////////////////////////////

// Static text label widget.
class label_widget final : public text_widget {
  public:
	// Creates a label widget.
	label_widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb,
				 tr::sys::ttf_style style, float font_size, tr::rgba8 color = GRAY);

	// The tint of the widget.
	tweened_color tint;

	// Updates the widget.
	void tick() override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;
};

//////////////////////////////////////////////////////////// TEXT BUTTON WIDGET ///////////////////////////////////////////////////////////

// Text widget that can be interacted with as a button.
class text_button_widget : public text_widget {
  public:
	// Creates a text button widget.
	text_button_widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb,
					   font font, float font_size, status_callback status_cb, action_callback action_cb, sound action_sound);

	// Gets whether the widget is interactible (delegates to the status callback).
	bool interactible() const override;

	// Function executed when the widget is clicked or activated with enter (delegates to the action callback).
	void on_action() override;
	// Function executed when the widget is hovered.
	void on_hover() override;
	// Function executed when the widget is unhovered.
	void on_unhover() override;
	// Function executed when the widget is held.
	void on_held() override;
	// Function executed when the widget is unheld.
	void on_unheld() override;
	// Function executed when the widget is selected.
	void on_selected() override;
	// Function executed when the widget is unselected.
	void on_unselected() override;

	// Updates the widget.
	void tick() override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

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
class basic_numeric_input_widget final : private basic_numeric_input_widget_data<T, Digits>, public text_widget {
  public:
	// Creates a numeric input widget.
	basic_numeric_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, float font_size, ui_manager& ui, T& ref,
							   status_callback status_cb, validation_callback<T> validation_cb);

	// Gets whether the widget is interactible (delegates to the status callback).
	bool interactible() const override;
	// Gets whether the widget is writable (always true).
	bool writable() const override;

	// Function executed when the widget is clicked or activated with enter.
	void on_action() override;
	// Function executed when the widget is hovered.
	void on_hover() override;
	// Function executed when the widget is unhovered.
	void on_unhover() override;
	// Function executed when the widget is held.
	void on_held() override;
	// Function executed when the widget is unheld.
	void on_unheld() override;
	// Function executed when the widget is selected.
	void on_selected() override;
	// Function executed when the widget is unselected.
	void on_unselected() override;
	// Function executed when writing to the widget.
	void on_write(std::string_view input) override;
	// Function executed when pressing enter on the widget.
	void on_enter() override;
	// Function executed when erasing from the widget.
	void on_erase() override;
	// Function executed when clearing the widget.
	void on_clear() override;

	// Updates the widget.
	void tick() override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;
};

// Default numeric input widget formatter (uses std::from_chars and std::format).
template <class T, tr::template_string_literal Format, tr::template_string_literal BufferFormat> struct default_numeric_input_formatter {
	// Converts a string to a value.
	static void from_string(std::common_type_t<T, int>& out, std::string_view str);
	// Formats a value.
	static std::string to_string(T v);
	// Formats a string (used during input).
	static std::string to_string(std::string_view str);
};
// Input widget for most numeric variables.
template <class T, usize Digits, tr::template_string_literal Format = "{}", tr::template_string_literal BufferFormat = "{}">
using numeric_input_widget = basic_numeric_input_widget<T, Digits, default_numeric_input_formatter<T, Format, BufferFormat>>;

// Special interval formatter for a numeric input widget.
struct interval_formatter {
	// Converts a string to a value.
	static void from_string(ticks& out, std::string_view str);
	// Formats an interval.
	static std::string to_string(ticks v);
	// Formats an interval string (used during input).
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
template <usize S> class line_input_widget final : public input_buffer<S>, public text_widget {
  public:
	// Creates a line input widget.
	line_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, tr::sys::ttf_style style, float font_size,
					  status_callback status_cb, action_callback enter_cb, std::string_view initial_text = {});

	// Gets whether the widget is interactible (delegates to the status callback).
	bool interactible() const override;
	// Gets whether the widget is writable (always true).
	bool writable() const override;

	// Function executed when the widget is clicked or activated with enter.
	void on_action() override;
	// Function executed when the widget is hovered.
	void on_hover() override;
	// Function executed when the widget is unhovered.
	void on_unhover() override;
	// Function executed when the widget is held.
	void on_held() override;
	// Function executed when the widget is unheld.
	void on_unheld() override;
	// Function executed when the widget is selected.
	void on_selected() override;
	// Function executed when the widget is unselected.
	void on_unselected() override;
	// Function executed when writing to the widget.
	void on_write(std::string_view input) override;
	// Function executed when pressing enter on the widget.
	void on_enter() override;
	// Function executed when erasing from the widget.
	void on_erase() override;
	// Function executed when clearing the widget.
	void on_clear() override;
	// Function executed when copying from the widget.
	void on_copy() override;
	// Function executed when pasting to the widget.
	void on_paste() override;

	// Updates the widget.
	void tick() override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

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
template <usize S> class multiline_input_widget final : public input_buffer<S>, public text_widget {
  public:
	// Creates a multiline input widget.
	multiline_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, float width, u8 max_lines, float font_size,
						   status_callback status_cb);

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Gets whether the widget is interactible (delegates to the status callback).
	bool interactible() const override;
	// Gets whether the widget is writable (always true).
	bool writable() const override;

	// Function executed when the widget is clicked or activated with enter.
	void on_action() override;
	// Function executed when the widget is hovered.
	void on_hover() override;
	// Function executed when the widget is unhovered.
	void on_unhover() override;
	// Function executed when the widget is held.
	void on_held() override;
	// Function executed when the widget is unheld.
	void on_unheld() override;
	// Function executed when the widget is selected.
	void on_selected() override;
	// Function executed when the widget is unselected.
	void on_unselected() override;
	// Function executed when writing to the widget.
	void on_write(std::string_view input) override;
	// Function executed when pressing enter on the widget.
	void on_enter() override;
	// Function executed when erasing from the widget.
	void on_erase() override;
	// Function executed when clearing the widget.
	void on_clear() override;
	// Function executed when copying from the widget.
	void on_copy() override;
	// Function executed when pasting to the widget.
	void on_paste() override;

	// Updates the widget.
	void tick() override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

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
class image_widget final : public widget {
  public:
	// Creates an image widget.
	image_widget(tweened_position pos, tr::align alignment, ticks unhide_time, int priority, std::string_view file,
				 tr::opt_ref<u16> hue = std::nullopt);

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Adds the widget to the renderer.
	void add_to_renderer() override;

  private:
	// The image texture, don't immediately load into a GPU texture to ensure it can be asynchronously loaded.
	std::variant<tr::bitmap, tr::gfx::texture> m_texture;
	// Optional reference to a hue the image takes on.
	tr::opt_ref<u16> m_hue;
	// The drawing priority of the widget.
	int m_priority;
};

//////////////////////////////////////////////////////// PLAYER SKIN PREVIEW WIDGET ///////////////////////////////////////////////////////

// Widget used to display a player skin preview.
class player_skin_preview_widget final : public widget {
  public:
	// Creates a player skill preview widget.
	player_skin_preview_widget(tweened_position pos, tr::align alignment, ticks unhide_time, settings& pending_settings);

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Updates the previewed skin.
	void update_skin();

	// Releases the widget's graphical resources.
	void release_graphical_resources() override;
	// Updates the widget.
	void tick() override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

  private:
	// Tag representing the lack of a skin.
	struct no_skin {};
	// Tag representing an unavailable skin.
	struct unavailable_skin {};

	// Optional skin texture, don't immediately load into a GPU texture to ensure it can be asynchronously loaded.
	std::variant<no_skin, unavailable_skin, tr::bitmap, tr::gfx::texture> m_skin;
	// Reference to the pending settings.
	settings& m_pending_settings;
	// The rotation of the preview.
	tr::angle m_rotation;
};

/////////////////////////////////////////////////////////// COLOR PREVIEW WIDGET //////////////////////////////////////////////////////////

// Widget used to display a color preview.
class color_preview_widget final : public widget {
  public:
	// Creates a color preview widget.
	color_preview_widget(tweened_position pos, tr::align alignment, ticks unhide_time, u16& hue);

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Adds the widget to the renderer.
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
class arrow_widget final : public widget {
  public:
	// Creates an arrow widget.
	arrow_widget(tweened_position pos, tr::valign alignment, ticks unhide_time, arrow_type type, status_callback status_cb,
				 action_callback action_cb);

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Gets whether the widget is interactible (delegates to the status callback).
	bool interactible() const override;

	// Function executed when the widget is clicked or activated with enter (delegates to the action callback).
	void on_action() override;
	// Function executed when the widget is hovered.
	void on_hover() override;
	// Function executed when the widget is unhovered.
	void on_unhover() override;
	// Function executed when the widget is held.
	void on_held() override;
	// Function executed when the widget is unheld.
	void on_unheld() override;
	// Function executed when the widget is selected.
	void on_selected() override;
	// Function executed when the widget is unselected.
	void on_unselected() override;

	// Updates the widget.
	void tick() override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

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
class replay_playback_indicator_widget final : public widget {
  public:
	// Creates a replay playback indicator widget.
	replay_playback_indicator_widget(tweened_position pos, tr::align alignment, ticks unhide_time);

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Adds the widget to the renderer.
	void add_to_renderer() override;
};

///////////////////////////////////////////////////////////// GAMEMODE WIDGET /////////////////////////////////////////////////////////////

// Action callback type used by the gamemode widget.
using gamemode_widget_action_callback = std::function<void(const gamemode_with_path&)>;

// Must be initialized before text_button_widget, so is separated out into its own struct.
struct gamemode_widget_data {
	// The gamemode associated with the widget.
	std::optional<gamemode_with_path> gp;
};

// Button widget used to display replay information and select a replay to be played.
class gamemode_widget final : public gamemode_widget_data, public text_button_widget {
  public:
	// Creates a gamemode widget.
	gamemode_widget(tweened_position pos, tr::align alignment, ticks unhide_time, status_callback status_cb,
					gamemode_widget_action_callback action_cb, std::optional<gamemode_with_path> gamemode);
};

/////////////////////////////////////////////////////////////// SCORE WIDGET //////////////////////////////////////////////////////////////

// Widget used to display a time or score result.
class score_widget final : public text_widget {
  public:
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

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Adds the widget to the renderer.
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
class replay_widget final : private replay_widget_data, public text_button_widget {
  public:
	// Creates a replay widget.
	replay_widget(tweened_position pos, tr::align alignment, ticks unhide_time, replays_state& state,
				  std::optional<replay_map::const_iterator> replay_it);

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Adds the widget to the renderer.
	void add_to_renderer() override;
};

////////////////////////////////////////////////////////////// IMPLEMENTATION /////////////////////////////////////////////////////////////

#include "line_input_widget_impl.hpp"      // IWYU pragma: keep
#include "multiline_input_widget_impl.hpp" // IWYU pragma: keep
#include "numeric_input_widget_impl.hpp"   // IWYU pragma: keep