#pragma once
#include "../font_manager.hpp"
#include "../replay.hpp"
#include "interpolated.hpp"

// Base UI widget template.
class widget {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a widget.
	widget(string&& name, vec2 pos, align alignment) noexcept;
	// Virtual destructor.
	virtual ~widget() noexcept = default;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// The name of the widget.
	string name;
	// The position of the widget.
	interpolated_vec2 pos;
	// The alignment of the widget.
	align alignment;

	///////////////////////////////////////////////////////////////// SIZE ////////////////////////////////////////////////////////////////

	// Gets the size of the widget.
	virtual vec2 size() const = 0;
	// Gets the top-left corner of the widget.
	virtual vec2 tl() const noexcept;

	/////////////////////////////////////////////////////////////// OPACITY ///////////////////////////////////////////////////////////////

	// Gets the opacity of the widget.
	float opacity() const noexcept;
	// Hides the widget instantly.
	void hide() noexcept;
	// Hides the widget gradually.
	void hide(u16 time) noexcept;
	// Unhides the widget instantly.
	void unhide() noexcept;
	// Unhides the widget gradually.
	void unhide(u16 time) noexcept;

	/////////////////////////////////////////////////////////////// STATUS ////////////////////////////////////////////////////////////////

	// Gets whether the widget is active.
	virtual bool active() const noexcept;

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
using tooltip_callback = function<string()>;
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

// Shortcut key chord.
struct key_chord {
	// Shortcut chord key.
	key key;
	// Shortcut chord modifiers.
	mods mods{mods::NONE};
};

// Interface for a shortcutable widget.
struct shortcutable {
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a shortcutable.
	shortcutable(vector<key_chord>&& chords) noexcept;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// The list of key chords of the shortcuts of the widget.
	vector<key_chord> chords;

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
	virtual void on_write(string_view input) = 0;
	// Callback for when enter is pressed.
	virtual void on_enter() = 0;
	// Callback for character deletion.
	virtual void on_erase() = 0;
	// Callback for input clearing.
	virtual void on_clear() = 0;
	// Callback for text copying.
	virtual void on_copy() = 0;
	// Callback for text pasting.
	virtual void on_paste() = 0;
};

/////////////////////////////////////////////////////////////// TEXT WIDGETS //////////////////////////////////////////////////////////////

// Alias for a text widget text callback.
using text_callback = function<string(const string&)>;
// Default callback for text widgets: gettnig a localization string using the widget name as the key.
inline const text_callback DEFAULT_TEXT_CALLBACK{[](const string& name) { return string{localization[name]}; }};

// Widget for basic, non-interactable text.
class basic_text_widget : public widget {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a basic text widget.
	// The callback is a function object that takes in the name of the object and returns a string.
	basic_text_widget(string&& name, vec2 pos, align alignment, font font, float font_size, text_callback text_cb = DEFAULT_TEXT_CALLBACK,
					  rgba8 color = {160, 160, 160, 160}, ttf_style style = NORMAL) noexcept;

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The text tint color.
	interpolated_rgba8 color;

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Sets the text callback of the widget.
	void set_text_callback(text_callback text_cb) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Gets the size of the widget.
	vec2 size() const noexcept override;
	// Updates the widget.
	void update() noexcept override;
	// Instructs the widget to release its graphical resources.
	void release_graphical_resources() noexcept override;
	// Adds the widget to the renderer.
	void add_to_renderer() override;

  private:
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
	// The font size used when drawing the text.
	float _font_size;
	// The callback used to get the text.
	text_callback _text_cb;
	// Cached texture and string.
	optional<cached> _cached;

	// Updates the cache.
	void update_cache();
};

// Widget for tooltippable, but otherwise non-interactable text.
class tooltippable_text_widget : public basic_text_widget, public mousable {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a tooltippable text widget.
	tooltippable_text_widget(string&& name, vec2 pos, align alignment, string_view tooltip_key, float font_size) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	void on_hover() noexcept override {}
	void on_unhover() noexcept override {}
	void on_hold_begin() noexcept override {};
	void on_hold_transfer_in() noexcept override {};
	void on_hold_transfer_out() noexcept override {};
	void on_hold_end() noexcept override {};
};

// Alias for a clickable widget status callback.
using status_callback = function<bool()>;
// Alias for a clickable widget action callback.
using action_callback = function<void()>;

// Widget for clickable text.
class clickable_text_widget : public basic_text_widget, public mousable, public shortcutable {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a clickable text widget.
	clickable_text_widget(string&& name, vec2 pos, align alignment, float font_size, status_callback status_cb, action_callback action_cb,
						  vector<key_chord>&& chords = {}, tooltip_callback tooltip_cb = NO_TOOLTIP) noexcept;
	// Creates a clickable text widget.
	clickable_text_widget(string&& name, vec2 pos, align alignment, font font, float font_size, status_callback status_cb,
						  action_callback action_cb, text_callback text_cb = DEFAULT_TEXT_CALLBACK, vector<key_chord>&& chords = {},
						  tooltip_callback tooltip_cb = NO_TOOLTIP) noexcept;

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
class text_line_input_widget : public basic_text_widget, public mousable, public writable {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a text line input wiget.
	text_line_input_widget(string&& name, vec2 pos, align alignment, float font_size, status_callback status_cb, action_callback enter_cb,
						   u8 max_size, string&& starting_text = {});

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The input buffer.
	string buffer;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	void add_to_renderer() override;

	bool active() const noexcept override;
	void on_hover() noexcept override;
	void on_unhover() noexcept override;
	void on_hold_begin() noexcept override;
	void on_hold_transfer_in() noexcept override;
	void on_hold_transfer_out() noexcept override;
	void on_hold_end() noexcept override;

	void on_gain_focus() override;
	void on_lose_focus() override;
	void on_write(string_view input) override;
	void on_enter() override;
	void on_erase() override;
	void on_clear() override;
	void on_copy() override;
	void on_paste() override;

  private:
	// Callback used to determine the status of the widget.
	status_callback _status_cb;
	// Callback called when enter is pressed.
	action_callback _enter_cb;
	// Maximum allowed size of the buffer.
	u8 _max_size;
	// Keeps track of whether the widget has input focus.
	bool _has_focus;
};

///////////////////////////////////////////////////////////// NON-TEXT WIDGETS ////////////////////////////////////////////////////////////

// Color preview widget.
class color_preview_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a color preview widget.
	color_preview_widget(string&& name, vec2 pos, align alignment, u16& hue_ref) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	void add_to_renderer() override;

  private:
	// Reference to the hue value.
	u16& _hue_ref;
};

// Clickable arrow widget.
class arrow_widget : public widget, public mousable, public shortcutable {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates an arrow widget.
	arrow_widget(string&& name, vec2 pos, align alignment, bool right_arrow, status_callback status_cb, action_callback action_cb,
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
class replay_playback_indicator_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a replay playback indicator widget.
	replay_playback_indicator_widget(string&& name, vec2 pos, align alignment) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	void add_to_renderer() override;
};

////////////////////////////////////////////////////////////// MIXED WIDGETS //////////////////////////////////////////////////////////////

// Replay widget.
class replay_widget : public clickable_text_widget {
  public:
	replay_widget(string&& name, vec2 pos, align alignment, auto base_status_cb, auto base_action_cb,
				  optional<map<string, replay_header>::iterator> it, key shortcut)
		: clickable_text_widget{std::move(name),
								pos,
								alignment,
								font::LANGUAGE,
								40,
								[=, this] { return base_status_cb() && _it.has_value(); },
								[=, this] {
									if (_it.has_value()) {
										base_action_cb(*_it);
									}
								},
								[this](const string&) {
									if (!_it.has_value()) {
										return string{"."};
									}

									replay_header& rpy{(*_it)->second};
									const ticks result{rpy.result};
									const ch::system_clock::time_point utc_tp{ch::seconds{rpy.timestamp}};
									const auto tp{std::chrono::current_zone()->ch::time_zone::to_local(utc_tp)};
									const ch::hh_mm_ss hhmmss{tp - ch::floor<ch::days>(tp)};
									const ch::year_month_day ymd{ch::floor<ch::days>(tp)};
									return format("{} ({}{})\n{} | {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", rpy.name,
												  localization["by"], rpy.player, rpy.gamemode.name, result / 60_s, (result % 60_s) / 1_s,
												  (result % 1_s) * 100 / 1_s, static_cast<int>(ymd.year()),
												  static_cast<unsigned int>(ymd.month()), static_cast<unsigned int>(ymd.day()),
												  hhmmss.hours().count(), hhmmss.minutes().count());
								},
								{{shortcut}},
								[this] {
									if (!_it.has_value()) {
										return string{};
									}
									else {
										const replay_header& header{(*_it)->second};
										string str{header.description};
										if (header.flags.exited_prematurely || header.flags.modified_game_speed) {
											str.push_back('\n');
										}
										if (header.flags.exited_prematurely) {
											str.push_back('\n');
											str.append(localization["exited_prematurely"]);
										}
										if (header.flags.modified_game_speed) {
											str.push_back('\n');
											str.append(localization["modified_game_speed"]);
										}
										return str;
									}
								}}
		, _it{it}
	{
	}

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Sets the iterator of the widget.
	void set_iterator(optional<map<string, replay_header>::iterator> it) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	vec2 tl() const noexcept override;
	void add_to_renderer() override;

  private:
	// The replay header.
	optional<map<string, replay_header>::iterator> _it;
};