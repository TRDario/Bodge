#pragma once
#include "../audio.hpp"
#include "../fonts.hpp"
#include "../replay.hpp"
#include "widget_base.hpp"

class ui_manager;

/////////////////////////////////////////////////////////////// LABEL WIDGET //////////////////////////////////////////////////////////////

class label_widget : public text_widget {
  public:
	label_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb,
				 tr::system::ttf_style style, float font_size, tr::rgba8 color = "A0A0A0A0"_rgba8);

	tweener<tr::rgba8> color;

	void update() override;
	void add_to_renderer() override;
};

//////////////////////////////////////////////////////////// TEXT BUTTON WIDGET ///////////////////////////////////////////////////////////

class text_button_widget : public text_widget {
  public:
	text_button_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb,
					   font font, float font_size, status_callback status_cb, action_callback action_cb, sound action_sound);

	void update() override;
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
	status_callback m_scb;
	action_callback m_acb;
	sound m_action_sound;

  protected:
	tweener<tr::rgba8> m_interp;

  private:
	bool m_hovered;
	bool m_held;
	bool m_selected;
	ticks m_action_left;
};

/////////////////////////////////////////////////////////// NUMERIC INPUT WIDGET //////////////////////////////////////////////////////////

template <class T, tr::template_string_literal Fmt, tr::template_string_literal BufferFmt> struct default_numeric_input_formatter {
	static void from_string(std::common_type_t<T, int>& out, std::string_view str);
	static std::string to_string(T v);
	static std::string to_string(std::string_view str);
};

template <class T, usize S, class Formatter> class basic_numeric_input_widget : public text_widget {
  public:
	basic_numeric_input_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, float font_size, ui_manager& ui, T& ref,
							   status_callback status_cb, validation_callback<T> validation_cb);

	void add_to_renderer() override;
	void update() override;
	bool interactible() const override;

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

  private:
	ui_manager& m_ui;
	T& m_ref;
	tr::static_string<S> m_buffer;
	status_callback m_scb;
	validation_callback<T> m_vcb;
	tweener<tr::rgba8> m_interp;
	bool m_hovered;
	bool m_held;
	bool m_selected;
};

template <class T, usize S, tr::template_string_literal Fmt, tr::template_string_literal BufferFmt>
using numeric_input_widget = basic_numeric_input_widget<T, S, default_numeric_input_formatter<T, Fmt, BufferFmt>>;

//////////////////////////////////////////////////////////// LINE INPUT WIDGET ////////////////////////////////////////////////////////////

template <usize S> class line_input_widget : public text_widget {
  public:
	line_input_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, tr::system::ttf_style style, float font_size,
					  status_callback status_cb, action_callback enter_cb, std::string_view initial_text);

	tr::static_string<S * 4> buffer;

	void add_to_renderer() override;
	void update() override;
	bool interactible() const override;

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
	status_callback m_scb;
	action_callback m_enter_cb;
	tweener<tr::rgba8> m_interp;
	bool m_hovered;
	bool m_held;
	bool m_selected;
};

////////////////////////////////////////////////////////// MULTILINE INPUT WIDGET /////////////////////////////////////////////////////////

template <usize S> class multiline_input_widget : public text_widget {
  public:
	multiline_input_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, float width, u8 max_lines, float font_size,
						   status_callback status_cb);

	tr::static_string<S * 4> buffer;

	glm::vec2 size() const override;
	void add_to_renderer() override;
	void update() override;
	bool interactible() const override;

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
	status_callback m_scb;
	glm::vec2 m_size;
	u8 m_max_lines;
	tweener<tr::rgba8> m_interp;
	bool m_hovered;
	bool m_held;
	bool m_selected;
};

/////////////////////////////////////////////////////////////// IMAGE WIDGET //////////////////////////////////////////////////////////////

class image_widget : public widget {
  public:
	image_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, int priority, std::string_view file,
				 tr::opt_ref<u16> hue = std::nullopt);

	glm::vec2 size() const override;
	void add_to_renderer() override;

  private:
	tr::gfx::texture m_texture;
	tr::opt_ref<u16> m_hue;
	int m_priority;
};

/////////////////////////////////////////////////////////// COLOR PREVIEW WIDGET //////////////////////////////////////////////////////////

class color_preview_widget : public widget {
  public:
	color_preview_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, u16& hue);

	glm::vec2 size() const override;
	void add_to_renderer() override;

  private:
	u16& m_hue;
};

/////////////////////////////////////////////////////////////// ARROW WIDGET //////////////////////////////////////////////////////////////

class arrow_widget : public widget {
  public:
	arrow_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, bool right_arrow, status_callback status_cb,
				 action_callback action_cb);

	glm::vec2 size() const override;
	void add_to_renderer() override;
	void update() override;

	bool interactible() const override;
	void on_action() override;
	void on_hover() override;
	void on_unhover() override;
	void on_held() override;
	void on_unheld() override;
	void on_selected() override;
	void on_unselected() override;

  protected:
	status_callback m_scb;
	action_callback m_acb;
	tweener<tr::rgba8> m_interp;
	bool m_right;
	bool m_hovered;
	bool m_held;
	bool m_selected;
	ticks m_action_left;
};

/////////////////////////////////////////////////////////////// ARROW WIDGET //////////////////////////////////////////////////////////////

struct replay_playback_indicator_widget : public widget {
	replay_playback_indicator_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time);

	glm::vec2 size() const override;
	void add_to_renderer() override;
};

/////////////////////////////////////////////////////////////// SCORE WIDGET //////////////////////////////////////////////////////////////

struct score_widget : public text_widget {
	static constexpr usize DONT_SHOW_RANK{std::numeric_limits<usize>::max()};

	score_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, usize rank, tr::opt_ref<score_entry> score);

	usize rank;
	tr::opt_ref<score_entry> score;

	glm::vec2 size() const override;
	void add_to_renderer() override;
};

////////////////////////////////////////////////////////////// REPLAY WIDGET //////////////////////////////////////////////////////////////

using replay_widget_action_callback = std::function<void(std::map<std::string, replay_header>::iterator)>;

struct replay_widget : public text_button_widget {
	replay_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, status_callback scb, replay_widget_action_callback acb,
				  std::optional<std::map<std::string, replay_header>::iterator> it);

	std::optional<std::map<std::string, replay_header>::iterator> it;

	glm::vec2 size() const override;
	void add_to_renderer() override;
};

#include "line_input_widget_impl.hpp"
#include "multiline_input_widget_impl.hpp"
#include "numeric_input_widget_impl.hpp"