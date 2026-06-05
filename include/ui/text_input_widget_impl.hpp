///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements text_input_widget from ui/widget_base.hpp.                                                                                 //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../audio.hpp"
#include "widget_base.hpp"

//////////////////////////////////////////////////////////// TEXT INPUT WIDGET ////////////////////////////////////////////////////////////

template <usize MaxChars>
text_input_widget<MaxChars>::text_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, tr::sys::ttf_style style,
											   float font_size, int width, status_command status_command, std::string_view initial_text)
	: input_buffer<MaxChars>{initial_text}
	, text_widget{pos, alignment, unhide_time, NO_TOOLTIP, buffer_text{this->m_buffer}, font::LANGUAGE, style, font_size, width}
	, m_status{std::move(status_command)}
	, m_tint{m_status() ? GRAY : DISABLED_GRAY}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
{
}

template <usize MaxChars>
text_input_widget<MaxChars>::text_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, tr::sys::ttf_style style,
											   float font_size, int width, status_command status_command, text_command text)
	: input_buffer<MaxChars>{}
	, text_widget{pos, alignment, unhide_time, NO_TOOLTIP, std::move(text), font::LANGUAGE, style, font_size, width}
	, m_status{std::move(status_command)}
	, m_tint{m_status() ? GRAY : DISABLED_GRAY}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
{
}

//

template <usize MaxChars> void text_input_widget<MaxChars>::tick()
{
	text_widget::tick();
	m_tint.tick();

	if (interactible()) {
		// Change color back to unselected gray if not selected/hovered/held and not already gray.
		if (interactible() && !(m_held || m_hovered || m_selected) && m_tint.done() && m_tint != GRAY) {
			m_tint.change(GRAY, 0.1_s);
		}
		// Slowly cycle between gray and a slight primary hue tint if hovered/selected.
		else if (m_tint.done() && (m_hovered || m_selected) && !m_held) {
			m_tint.change(tr::color_cast<tr::rgba8>(tr::hsv{float(active_settings::instance()->primary_hue), 0.2f, 1.0f}), 4_s, cycle::YES);
		}
	}
	else {
		// Cannot be hovered/held/selected when disabled, set all those to false and change color to disabled gray.
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if ((m_tint.done() && m_tint != DISABLED_GRAY) || m_tint.cycling()) {
			m_tint.change(DISABLED_GRAY, 0.1_s);
		}
	}
}

//

template <usize MaxChars> bool text_input_widget<MaxChars>::interactible() const
{
	return m_status();
}

template <usize MaxChars> bool text_input_widget<MaxChars>::writable() const
{
	return true;
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_action()
{
	m_tint.change(WHITE, 0.1_s);
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_tint.change(WHITE, 0.1_s);
			g_audio.play_sound(sound::HOVER, 0.15f, 0.0f, g_rng.generate(0.9f, 1.1f));
		}
	}
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected) {
			m_tint.change(GRAY, 0.1_s);
		}
	}
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_held()
{
	if (interactible()) {
		m_held = true;
		m_tint = HELD_GRAY;
	}
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_tint.change(WHITE, 0.1_s);
		}
		else {
			g_audio.play_sound(sound::CONFIRM, 0.5f, 0.0f, g_rng.generate(0.9f, 1.1f));
		}
	}
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered) {
			m_tint.change(GRAY, 0.1_s);
		}
	}
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_erase()
{
	if (!this->m_buffer.empty()) {
		tr::utf8::pop_back(this->m_buffer);
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_clear()
{
	this->m_buffer.clear();
	g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
}

template <usize MaxChars> void text_input_widget<MaxChars>::on_copy()
{
	tr::sys::set_clipboard_text(std::string{this->m_buffer});
}

//

template <usize MaxChars> text text_input_widget<MaxChars>::text() const
{
	return {this->m_buffer, m_font, m_style, m_font_size, m_font_size / 12, float(m_max_width)};
}