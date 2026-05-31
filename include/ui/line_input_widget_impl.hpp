///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements line_input_widget from ui/widget.hpp.                                                                                      //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "widget.hpp"

//////////////////////////////////////////////////////////// LINE INPUT WIDGET ////////////////////////////////////////////////////////////

template <usize MaxChars>
line_input_widget<MaxChars>::line_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, tr::sys::ttf_style style,
											   float font_size, status_command status_command, action_command enter_action_command,
											   std::string_view initial_text)
	: text_input_widget<MaxChars * 4>{
		  pos, alignment, unhide_time, style, font_size, tr::sys::UNLIMITED_WIDTH, std::move(status_command), initial_text,
	  }
	, m_enter_action{std::move(enter_action_command)}
{
}

//

template <usize MaxChars> std::string_view line_input_widget<MaxChars>::contents() const
{
	return this->m_buffer;
}

//

template <usize MaxChars> void line_input_widget<MaxChars>::on_write(std::string_view input)
{
	if (tr::utf8::length(this->m_buffer) + tr::utf8::length(input) <= MaxChars) {
		this->m_buffer.append(input);
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <usize MaxChars> void line_input_widget<MaxChars>::on_enter()
{
	m_enter_action();
	g_audio.play_sound(sound::CONFIRM, 0.5f, 0.0f, g_rng.generate(0.9f, 1.1f));
}

template <usize MaxChars> void line_input_widget<MaxChars>::on_paste()
{
	const usize current_length{tr::utf8::length(this->m_buffer)};
	if (!tr::sys::clipboard_empty() && current_length < MaxChars) {
		// Remove all newlines in the clipboard string.
		std::string clipboard{tr::sys::clipboard_text()};
		std::erase(clipboard, '\n');
		const usize clipboard_length{tr::utf8::length(clipboard)};

		// Paste the clipboard content into a copy of the buffer, clip the pasted text if it would overflow.
		const ssize overflow{ssize(current_length + clipboard_length - MaxChars)};
		const auto end{(overflow > 0) ? tr::utf8::next(clipboard.begin(), clipboard_length - overflow) : clipboard.end()};
		this->m_buffer.append(clipboard.begin(), end);

		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

//

template <usize MaxChars> void line_input_widget<MaxChars>::add_to_renderer()
{
	const tr::rgba8 tint{this->m_tint};
	text_widget::add_to_renderer_raw(this->m_buffer.empty() ? tr::rgba8{u8(tint.r / 2), u8(tint.g / 2), u8(tint.b / 2), 255} : tint);
}