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
	// Label properties.
	struct properties {
		// Initial position (or animation) of the label.
		tweened_position animation;
		// Alignment of the label.
		tr::align alignment{tr::align::CENTER};
		// Amount of time it takes to unhide the label.
		ticks unhide_time{0.5_s};
		// Command used to fetch the tooltip text of the label.
		text_command tooltip_text{NO_TOOLTIP};
		// Command used to fetch the text of the label.
		text_command text;
		// Font style of the label.
		tr::sys::ttf_style font_style{tr::sys::ttf_style::NORMAL};
		// Font size of the label.
		float font_size{48};
		// Color of the label.
		tr::rgba8 color{GRAY};
	};

	// Creates a label widget.
	label_widget(properties&& properties);

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
	// Text button properties.
	struct properties {
		// Initial position (or animation) of the button.
		tweened_position animation;
		// Alignment of the button.
		tr::align alignment{tr::align::CENTER};
		// Amount of time it takes to unhide the button.
		ticks unhide_time{0.5_s};
		// Command used to fetch the tooltip text of the button.
		text_command tooltip_text{NO_TOOLTIP};
		// Command used to fetch the text of the button.
		text_command text;
		// Font of the button.
		font font{font::LANGUAGE};
		// Font style of the button.
		tr::sys::ttf_style font_style{tr::sys::ttf_style::NORMAL};
		// Font size of the button.
		float font_size{48};
		// Command used to query the status of the button.
		status_command status;
		// Command called when the button is pressed.
		action_command action;
		// Sound played during an action.
		sound action_sound{sound::CONFIRM};
	};

	// Creates a text button widget.
	text_button_widget(properties&& properties);

	// Gets whether the widget is interactible (delegates to the status command).
	bool interactible() const override;

	// Function executed when the widget is clicked or activated with enter (delegates to the action command).
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
	// Command used to determine whether the button is interactible.
	status_command m_status;
	// Action executed when the button is interacted with.
	action_command m_action;
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
template <class T> struct basic_numeric_input_widget_data {
	// Reference to the UI manager.
	ui_manager& m_ui;
	// Reference to the variable the widget is bound to.
	T& m_bound_variable;
	// Command used to validate the value after input is finished.
	validation_command<T> m_validator;
};

// Widget used to input a numeric value.
// Formatter is a class with static members from_string() (string -> value) and to_string() (value/string_view -> string).
template <class T, usize Digits, class Formatter>
class basic_numeric_input_widget final : private basic_numeric_input_widget_data<T>, public text_input_widget<Digits> {
  public:
	// Creates a numeric input widget.
	basic_numeric_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, float font_size, ui_manager& ui, T& ref,
							   status_command status_command, validation_command<T> validation_command);

	// Function executed when the widget is selected.
	void on_selected() override;
	// Function executed when the widget is unselected.
	void on_unselected() override;
	// Function executed when writing to the widget.
	void on_write(std::string_view input) override;
	// Function executed when pressing enter on the widget.
	void on_enter() override;

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

// Widget used to input a line of text.
template <usize MaxChars> class line_input_widget final : public text_input_widget<MaxChars * 4> {
  public:
	// Creates a line input widget.
	line_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, tr::sys::ttf_style style, float font_size,
					  status_command status_command, action_command enter_action_command, std::string_view initial_text = {});

	// Gets the contents of the widget.
	std::string_view contents() const;

	// Function executed when writing to the widget.
	void on_write(std::string_view input) override;
	// Function executed when pressing enter on the widget.
	void on_enter() override;
	// Function executed when pasting to the widget.
	void on_paste() override;

	// Adds the widget to the renderer.
	void add_to_renderer() override;

  private:
	// Function called when enter is pressed while the widget is selected.
	action_command m_enter_action;
};

////////////////////////////////////////////////////////// MULTILINE INPUT WIDGET /////////////////////////////////////////////////////////

// Widget used to input multiple lines of text.
template <usize MaxChars> class multiline_input_widget final : public text_input_widget<MaxChars * 4> {
  public:
	// Creates a multiline input widget.
	multiline_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, float width, u8 max_lines, float font_size,
						   status_command status_command);

	// Gets the contents of the widget.
	std::string_view contents() const;

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Function executed when writing to the widget.
	void on_write(std::string_view input) override;
	// Function executed when pressing enter on the widget.
	void on_enter() override;
	// Function executed when pasting to the widget.
	void on_paste() override;

	// Adds the widget to the renderer.
	void add_to_renderer() override;

  private:
	// The thickness of the outline of the widget.
	static constexpr float OUTLINE_THICKNESS{2.0f};

	// Size of the input box.
	glm::vec2 m_size;
	// Maximum allowed number of lines in the input.
	u8 m_max_lines;
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
	// Arrow widget properties.
	struct properties {
		// Initial animation of the widget.
		tweened_position animation;
		// Vertical alignment of the arrow widget.
		tr::valign alignment{tr::valign::CENTER};
		// Amount of time it takes to unhide the label.
		ticks unhide_time{0.5_s};
		// Arrow type.
		arrow_type type;
		// Command used to query the status of the widget.
		status_command status;
		// Command called when the arrow is pressed.
		action_command action;
	};

	// Creates an arrow widget.
	arrow_widget(properties&& properties);

	// Gets the size of the widget.
	glm::vec2 size() const override;

	// Gets whether the widget is interactible (delegates to the status command).
	bool interactible() const override;

	// Function executed when the widget is clicked or activated with enter (delegates to the action command).
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
	// Command used to determine whether the arrow is interactible.
	status_command m_status;
	// Action executed when the arrow is interacted with.
	action_command m_action;
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

// Action command type used by the gamemode widget.
using gamemode_widget_action_command = std::function<void(const gamemode_with_path&)>;

// Must be initialized before text_button_widget, so is separated out into its own struct.
struct gamemode_widget_data {
	// The gamemode associated with the widget.
	std::optional<gamemode_with_path> gp;
};

// Button widget used to display replay information and select a replay to be played.
class gamemode_widget final : public gamemode_widget_data, public text_button_widget {
  public:
	// Creates a gamemode widget.
	gamemode_widget(tweened_position pos, tr::align alignment, ticks unhide_time, status_command status_command,
					gamemode_widget_action_command action_command, std::optional<gamemode_with_path> gamemode);
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