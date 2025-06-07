#pragma once
#include "../font_manager.hpp"
#include "../replay.hpp"
#include "interpolated.hpp"

// Shortcut key chord.
struct key_chord {
	// Shortcut chord key.
	key key;
	// Shortcut chord modifiers.
	mods mods{mods::NONE};

	bool operator==(const key_chord&) const = default;
};

// Alias for the type of a tooltip callback.
using tooltip_callback = function<string()>;
// Sentinel for a mousable to not have a tooltip.
inline const tooltip_callback NO_TOOLTIP{};

// Base widget class.
class widget {
  public:
	// Creates a widget.
	widget(string_view name, vec2 pos, align alignment, bool hoverable, tooltip_callback tooltip_cb, bool writable,
		   vector<key_chord>&& shortcuts) noexcept;
	// Virtual destructor.
	virtual ~widget() noexcept = default;

	////////////////////////////////////////////////////////////// ATTRIBUTES /////////////////////////////////////////////////////////////

	// The name of the widget.
	static_string<30> name;
	// The alignment of the widget.
	align alignment;
	// The position of the widget.
	interpolated_vec2 pos;
	// An optional tooltip callback (the widget must be hoverable for it to be used).
	tooltip_callback tooltip_cb;

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
	virtual void on_write(string_view){};
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
	vector<key_chord> _shortcuts;
};

/////////////////////////////////////////////////////////////// TEXT WIDGETS //////////////////////////////////////////////////////////////

// Alias for a text widget text callback.
using text_callback = function<string(const static_string<30>&)>;
// Default callback for text widgets: gettnig a localization string using the widget name as the key.
inline const text_callback DEFAULT_TEXT_CALLBACK{[](const static_string<30>& name) { return string{localization[name]}; }};

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

// Alias for a clickable widget status callback.
using status_callback = function<bool()>;
// Alias for a clickable widget action callback.
using action_callback = function<void()>;

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
	line_input_widget(string_view name, vec2 pos, align alignment, float font_size, status_callback status_cb, action_callback enter_cb,
					  string_view starting_text = {}) noexcept
		: text_widget{name,
					  pos,
					  alignment,
					  true,
					  NO_TOOLTIP,
					  true,
					  {},
					  font::LANGUAGE,
					  ttf_style::NORMAL,
					  halign::CENTER,
					  font_size,
					  UNLIMITED_WIDTH,
					  {160, 160, 160, 160},
					  [this](const static_string<30>&) { return buffer.empty() ? string{localization["empty"]} : string{buffer}; }}
		, buffer{std::move(starting_text)}
		, _status_cb{std::move(status_cb)}
		, _enter_cb{std::move(enter_cb)}
	{
	}

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The input buffer.
	static_string<S> buffer;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	void add_to_renderer() override
	{
		interpolated_rgba8 real_color{color};
		if (!active()) {
			color = {80, 80, 80, 160};
		}
		else if (buffer.empty()) {
			rgba8 real{real_color};
			color = {static_cast<u8>(real.r / 2), static_cast<u8>(real.g / 2), static_cast<u8>(real.b / 2), real.a};
		}
		text_widget::add_to_renderer();
		color = real_color;
	}

	bool active() const noexcept override
	{
		return _status_cb();
	}

	void on_hover() noexcept override
	{
		if (!_has_focus) {
			color.change({220, 220, 220, 220}, 0.2_s);
		}
	}

	void on_unhover() noexcept override
	{
		if (!_has_focus) {
			color.change({160, 160, 160, 160}, 0.2_s);
		}
	}

	void on_hold_begin() noexcept override
	{
		color = {32, 32, 32, 255};
	}

	void on_hold_transfer_in() noexcept override
	{
		color = {32, 32, 32, 255};
	}

	void on_hold_transfer_out() noexcept override
	{
		color = rgba8{160, 160, 160, 160};
	}

	void on_hold_end() noexcept override
	{
		_has_focus = true;
		color = {255, 255, 255, 255};
	}

	void on_gain_focus() override
	{
		_has_focus = true;
		color.change({255, 255, 255, 255}, 0.2_s);
	}

	void on_lose_focus() override
	{
		_has_focus = false;
		color.change({160, 160, 160, 160}, 0.2_s);
	}

	void on_write(string_view input) override
	{
		if (buffer.size() + input.size() <= S) {
			buffer.append(input);
		}
	}

	void on_enter() override
	{
		_enter_cb();
	}

	void on_erase() override
	{
		buffer.pop_back();
	}

	void on_clear() override
	{
		buffer.clear();
	}

	void on_copy() override
	{
		tr::keyboard::set_clipboard_text(string{buffer}.c_str());
	}

	void on_paste() override
	{
		if (tr::keyboard::clipboard_has_text()) {
			string pasted{tr::keyboard::clipboard_text()};
			buffer += (buffer.size() + pasted.size() > S) ? string_view{pasted}.substr(0, S - buffer.size()) : pasted;
		}
	}

  private:
	// Callback used to determine the status of the widget.
	status_callback _status_cb;
	// Callback called when enter is pressed.
	action_callback _enter_cb;
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
class replay_playback_indicator_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a replay playback indicator widget.
	replay_playback_indicator_widget(string_view name, vec2 pos, align alignment) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	void add_to_renderer() override;
};

////////////////////////////////////////////////////////////// MIXED WIDGETS //////////////////////////////////////////////////////////////

// Score widget.
class score_widget : public text_widget {
  public:
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
class replay_widget : public clickable_text_widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	replay_widget(string_view name, vec2 pos, align alignment, auto base_status_cb, auto base_action_cb,
				  optional<map<string, replay_header>::iterator> it, key shortcut)
		: clickable_text_widget{name,
								pos,
								alignment,
								font::LANGUAGE,
								40,
								[this](const static_string<30>&) {
									if (!this->it.has_value()) {
										return string{"----------------------------------"};
									}

									replay_header& rpy{(*this->it)->second};
									const ticks result{rpy.result};
									const ch::system_clock::time_point utc_tp{ch::seconds{rpy.timestamp}};
									const auto tp{std::chrono::current_zone()->ch::time_zone::to_local(utc_tp)};
									const ch::hh_mm_ss hhmmss{tp - ch::floor<ch::days>(tp)};
									const ch::year_month_day ymd{ch::floor<ch::days>(tp)};
									return format("{} ({}{})\n{} | {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", rpy.name,
												  localization["by"], rpy.player,
												  rpy.gamemode.builtin ? localization[rpy.gamemode.name] : string_view{rpy.gamemode.name},
												  result / 60_s, (result % 60_s) / 1_s, (result % 1_s) * 100 / 1_s,
												  static_cast<int>(ymd.year()), static_cast<unsigned int>(ymd.month()),
												  static_cast<unsigned int>(ymd.day()), hhmmss.hours().count(), hhmmss.minutes().count());
								},
								[=, this] { return base_status_cb() && this->it.has_value(); },
								[=, this] {
									if (this->it.has_value()) {
										base_action_cb(*this->it);
									}
								},
								[this] {
									if (!this->it.has_value()) {
										return string{};
									}
									else {
										const replay_header& header{(*this->it)->second};
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
								},
								{{shortcut}}}
		, it{it}
	{
	}

	///////////////////////////////////////////////////////////// ATTRIBUTES //////////////////////////////////////////////////////////////

	// The replay header.
	optional<map<string, replay_header>::iterator> it;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	vec2 size() const noexcept override;
	vec2 tl() const noexcept override;
	void add_to_renderer() override;

  private:
};