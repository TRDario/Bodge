///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements numeric_input_widget from ui/widget.hpp.                                                                                   //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../ui.hpp"
#include "widget.hpp"

///////////////////////////////////////////////////// DEFAULT NUMERIC INPUT FORMATTER /////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////// NUMERIC INPUT WIDGET //////////////////////////////////////////////////////////

template <class T, usize Digits, class Formatter>
basic_numeric_input_widget<T, Digits, Formatter>::basic_numeric_input_widget(properties&& properties)
	: basic_numeric_input_widget_data<T>{properties.ui, properties.variable, std::move(properties.validation)}
	, text_input_widget<Digits>{
		  properties.animation,
		  properties.alignment,
		  properties.unhide_time,
		  tr::sys::ttf_style::NORMAL,
		  properties.font_size,
		  tr::sys::UNLIMITED_WIDTH,
		  std::move(properties.status),
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
				  return Formatter::to_string(this->m_bound_variable);
			  }
		  },
	  }
{
}

//

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_selected()
{
	text_input_widget<Digits>::on_selected();
	if (this->interactible()) {
		this->m_buffer.clear();
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_unselected()
{
	text_input_widget<Digits>::on_unselected();
	if (this->interactible()) {
		// Promote small integers to int so writing e.g. '435' clamps nicely to '255' instead of failing.
		std::common_type_t<T, int> raw_value{this->m_bound_variable};
		Formatter::from_string(raw_value, this->m_buffer);
		this->m_bound_variable = this->m_validator(raw_value);
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_write(std::string_view input)
{
	if (input.size() == 1 && (std::isdigit(input.front()) || input.front() == '.') && this->m_buffer.size() < Digits) {
		this->m_buffer.append(input);
		audio::instance().play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::on_enter()
{
	this->m_ui.clear_selection();
	audio::instance().play_sound(sound::CONFIRM, 0.5f, 0.0f, g_rng.generate(0.9f, 1.1f));
}

//

template <class T, usize Digits, class Formatter> void basic_numeric_input_widget<T, Digits, Formatter>::add_to_renderer(renderer& renderer)
{
	text_widget::add_to_renderer_raw(renderer, this->m_tint);
}