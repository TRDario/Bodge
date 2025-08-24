#pragma once
#include "ui_manager.hpp"
#include "widget.hpp"

template <class T, tr::template_string_literal Fmt, tr::template_string_literal BufferFmt>
void default_numeric_input_formatter<T, Fmt, BufferFmt>::from_string(std::common_type_t<T, int>& out, std::string_view str)
{
	std::from_chars(str.data(), str.data() + str.size(), out);
}

template <class T, tr::template_string_literal Fmt, tr::template_string_literal BufferFmt>
std::string default_numeric_input_formatter<T, Fmt, BufferFmt>::to_string(T v)
{
	return TR_FMT::format(Fmt, v);
}

template <class T, tr::template_string_literal Fmt, tr::template_string_literal BufferFmt>
std::string default_numeric_input_formatter<T, Fmt, BufferFmt>::to_string(std::string_view str)
{
	return TR_FMT::format(BufferFmt, str);
}

template <class T, usize S, class Formatter>
basic_numeric_input_widget<T, S, Formatter>::basic_numeric_input_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, float font_size,
												 ui_manager& ui, T& ref, status_callback status_cb,
												 validation_callback<T> validation_cb)
	: text_widget{
		  pos,
		  alignment,
		  unhide_time,
		  NO_TOOLTIP,
		  true,
		  [this] {
			  if (m_selected) {
				  if (m_buffer.empty()) {
					  return std::string{"..."};
				  }
				  else {
					  return Formatter::to_string(m_buffer);
				  }
			  }
			  else {
				  return Formatter::to_string(m_ref);
			  }
		  },
		  font::LANGUAGE,
		  tr::system::ttf_style::NORMAL,
		  font_size,
		  tr::system::UNLIMITED_WIDTH,
	  }
	  , m_ui{ui}
	  , m_ref{ref}
	  , m_scb{std::move(status_cb)}
	  , m_vcb{std::move(validation_cb)}
	  , m_interp{m_scb() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8}
	  , m_hovered{false}
	  , m_held{false}
	  , m_selected{false}
{
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::add_to_renderer()
{
	text_widget::add_to_renderer_raw(m_interp);
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::update()
{
	text_widget::update();
	m_interp.update();

	if (interactible()) {
		if (interactible() && !(m_held || m_hovered || m_selected) && m_interp.done() && m_interp != "A0A0A0A0"_rgba8) {
			m_interp.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
		else if (m_interp.done() && (m_hovered || m_selected) && !m_held) {
			m_interp.change(tween::CYCLE, tr::color_cast<tr::rgba8>(tr::hsv{float(engine::settings.primary_hue), 0.2f, 1.0f}), 4_s);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if ((m_interp.done() && m_interp != "505050A0"_rgba8) || m_interp.cycling()) {
			m_interp.change(tween::LERP, "505050A0"_rgba8, 0.1_s);
		}
	}
}

template <class T, usize S, class Formatter> bool basic_numeric_input_widget<T, S, Formatter>::interactible() const
{
	return m_scb();
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_action()
{
	m_interp.change(tween::LERP, "FFFFFF"_rgba8, 0.1_s);
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(tween::LERP, "FFFFFF"_rgba8, 0.1_s);
			engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected) {
			m_interp.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_held()
{
	if (interactible()) {
		m_held = true;
		m_interp = "202020"_rgba8;
	}
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_interp.change(tween::LERP, "FFFFFF"_rgba8, 0.1_s);
		}
		else {
			engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}

		m_buffer.clear();
	}
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered) {
			m_interp.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}

		// Promote small integers to int so writing e.g. '435' clamps nicely to '255' instead of failing.
		std::common_type_t<T, int> temp{m_ref};
		Formatter::from_string(temp, m_buffer);
		m_ref = m_vcb(temp);
	}
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_write(std::string_view input)
{
	if (input.size() == 1 && (std::isdigit(input.front()) || input.front() == '.') && m_buffer.size() < S) {
		m_buffer.append(input);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_enter()
{
	m_ui.clear_selection();
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_erase()
{
	if (!m_buffer.empty()) {
		m_buffer.pop_back();
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <class T, usize S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_clear()
{
	m_buffer.clear();
	engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
}