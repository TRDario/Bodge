#pragma once
#include "../ui.hpp"
#include "widget.hpp"

template <class T, tr::template_string_literal Format, tr::template_string_literal BufferFormat>
void default_numeric_input_formatter<T, Format, BufferFormat>::from_string(std::common_type_t<T, int>& out, std::string_view str)
{
	std::from_chars(str.data(), str.data() + str.size(), out);
}

template <class T, tr::template_string_literal Format, tr::template_string_literal BufferFormat>
std::string default_numeric_input_formatter<T, Format, BufferFormat>::to_string(T v)
{
	return TR_FMT::format(Format, v);
}

template <class T, tr::template_string_literal Format, tr::template_string_literal BufferFormat>
std::string default_numeric_input_formatter<T, Format, BufferFormat>::to_string(std::string_view str)
{
	return TR_FMT::format(BufferFormat, str);
}

template <class T, usize Digits, class Formatter>
basic_numeric_input_widget<T, Digits, Formatter>::basic_numeric_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time,
																			 float font_size, ui_manager& ui, T& ref,
																			 status_callback status_cb,
																			 validation_callback<T> validation_cb)
	: basic_numeric_input_widget_data<T,
									  Digits>{ui,    ref,   {},   status_cb, std::move(validation_cb), status_cb() ? GRAY : DISABLED_GRAY,
											  false, false, false}
	, text_widget{
		  pos,
		  alignment,
		  unhide_time,
		  NO_TOOLTIP,
		  [this] {
			  if (this->m_selected) {
				  if (this->m_buffer.empty()) {
					  return std::string{"..."};
				  }
				  else {
					  return Formatter::to_string(this->m_buffer);
				  }
			  }
			  else {
				  return Formatter::to_string(this->m_ref);
			  }
		  },
		  font::LANGUAGE,
		  tr::sys::ttf_style::NORMAL,
		  font_size,
		  tr::sys::UNLIMITED_WIDTH,
	  }
{
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::add_to_renderer()
{
	text_widget::add_to_renderer_raw(this->m_tint);
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::tick()
{
	text_widget::tick();
	this->m_tint.tick();

	if (interactible()) {
		if (interactible() && !(this->m_held || this->m_hovered || this->m_selected) && this->m_tint.done() && this->m_tint != GRAY) {
			this->m_tint.change(GRAY, 0.1_s);
		}
		else if (this->m_tint.done() && (this->m_hovered || this->m_selected) && !this->m_held) {
			this->m_tint.change(tr::color_cast<tr::rgba8>(tr::hsv{float(g_settings.primary_hue), 0.2f, 1.0f}), 4_s, cycle::YES);
		}
	}
	else {
		this->m_hovered = false;
		this->m_held = false;
		this->m_selected = false;
		if ((this->m_tint.done() && this->m_tint != DISABLED_GRAY) || this->m_tint.cycling()) {
			this->m_tint.change(DISABLED_GRAY, 0.1_s);
		}
	}
}

template <class T, usize Digits, class Formatter> bool basic_numeric_input_widget<T, Digits, Formatter>::interactible() const
{
	return this->m_scb();
}

template <class T, usize Digits, class Formatter> bool basic_numeric_input_widget<T, Digits, Formatter>::writable() const
{
	return true;
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_action()
{
	this->m_tint.change(WHITE, 0.1_s);
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_hover()
{
	if (interactible()) {
		this->m_hovered = true;
		if (!this->m_selected) {
			this->m_tint.change(WHITE, 0.1_s);
			g_audio.play_sound(sound::HOVER, 0.15f, 0.0f, g_rng.generate(0.9f, 1.1f));
		}
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_unhover()
{
	if (interactible()) {
		this->m_hovered = false;
		if (!this->m_selected) {
			this->m_tint.change(GRAY, 0.1_s);
		}
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_held()
{
	if (interactible()) {
		this->m_held = true;
		this->m_tint = HELD_GRAY;
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_unheld()
{
	if (interactible()) {
		this->m_held = false;
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_selected()
{
	if (interactible()) {
		this->m_selected = true;
		if (!this->m_hovered) {
			this->m_tint.change(WHITE, 0.1_s);
		}
		else {
			g_audio.play_sound(sound::CONFIRM, 0.5f, 0.0f, g_rng.generate(0.9f, 1.1f));
		}

		this->m_buffer.clear();
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_unselected()
{
	if (interactible()) {
		this->m_selected = false;
		if (!this->m_hovered) {
			this->m_tint.change(GRAY, 0.1_s);
		}

		// Promote small integers to int so writing e.g. '435' clamps nicely to '255' instead of failing.
		std::common_type_t<T, int> temp{this->m_ref};
		Formatter::from_string(temp, this->m_buffer);
		this->m_ref = this->m_vcb(temp);
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_write(std::string_view input)
{
	if (input.size() == 1 && (std::isdigit(input.front()) || input.front() == '.') && this->m_buffer.size() < Digits) {
		this->m_buffer.append(input);
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_enter()
{
	this->m_ui.clear_selection();
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_erase()
{
	if (!this->m_buffer.empty()) {
		this->m_buffer.pop_back();
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_clear()
{
	this->m_buffer.clear();
	g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
}