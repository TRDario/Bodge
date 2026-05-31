///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements multiline_input_widget from ui/widget.hpp.                                                                                 //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../renderer.hpp"
#include "widget.hpp"

////////////////////////////////////////////////////////// MULTILINE INPUT WIDGET /////////////////////////////////////////////////////////

template <usize MaxChars>
multiline_input_widget<MaxChars>::multiline_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, float width,
														 u8 max_lines, float font_size, status_command status_command)
	: text_input_widget<MaxChars * 4>{
		  pos, alignment, unhide_time, tr::sys::ttf_style::NORMAL, font_size, int(width), std::move(status_command),
	  }
	, m_size{width, g_text_engine.line_skip(font::LANGUAGE, font_size) * max_lines + 2 * OUTLINE_THICKNESS}
	, m_max_lines{max_lines}
{
}

//

template <usize MaxChars> std::string_view multiline_input_widget<MaxChars>::contents() const
{
	return this->m_buffer;
}

//

template <usize MaxChars> glm::vec2 multiline_input_widget<MaxChars>::size() const
{
	return m_size;
}

//

template <usize MaxChars> void multiline_input_widget<MaxChars>::on_write(std::string_view input)
{
	if (tr::utf8::length(this->m_buffer) + tr::utf8::length(input) <= MaxChars) {
		this->m_buffer.append(input);

		// Revert new addition if the new text is over the line limit.
		if (g_text_engine.count_lines(this->text()) > m_max_lines) {
			this->m_buffer.resize(this->m_buffer.size() - input.size());
		}
		else {
			g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
		}
	}
}

template <usize MaxChars> void multiline_input_widget<MaxChars>::on_enter()
{
	if (tr::utf8::length(this->m_buffer) < MaxChars && g_text_engine.count_lines(this->text()) < m_max_lines) {
		this->m_buffer.append('\n');
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <usize MaxChars> void multiline_input_widget<MaxChars>::on_paste()
{
	try {
		if (!tr::sys::clipboard_empty()) {
			const std::string clipboard{tr::sys::clipboard_text()};
			const usize clipboard_length{tr::utf8::length(clipboard)};

			// Paste the clipboard content into a copy of the buffer, clip the pasted text if it would overflow.
			tr::static_string new_string{this->m_buffer};
			const ssize overflow{ssize(tr::utf8::length(this->m_buffer) + clipboard_length - MaxChars)};
			const auto end{(overflow > 0) ? tr::utf8::next(clipboard.begin(), clipboard_length - overflow) : clipboard.end()};
			new_string.append(clipboard.begin(), end);

			// Reject the new string if it goes over the line limit (I realize it's not the most elegant solution).
			const text new_string_text{new_string,        font::LANGUAGE,         tr::sys::ttf_style::NORMAL,
									   this->m_font_size, this->m_font_size / 12, m_size.x};
			if (g_text_engine.count_lines(new_string_text) <= m_max_lines) {
				this->m_buffer = new_string;
			}
			else {
				g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
			}
		}
	}
	catch (...) {
	}
}

//

template <usize MaxChars> void multiline_input_widget<MaxChars>::add_to_renderer()
{
	const glm::vec2 tl{this->tl()};
	const tr::rgba8 tint{this->m_tint};
	const float opacity{this->opacity()};

	const tr::gfx::simple_color_mesh_ref background{g_renderer->basic.new_color_fan(layer::UI, 4)};
	tr::fill_rectangle_vertices(background.positions, {tl + OUTLINE_THICKNESS, size() - 2 * OUTLINE_THICKNESS});
	std::ranges::fill(background.colors, tr::rgba8{0, 0, 0, u8(160 * opacity)});

	text_widget::add_to_renderer_raw(this->m_buffer.empty() ? tr::rgba8{u8(tint.r / 2), u8(tint.g / 2), u8(tint.b / 2), 255} : tint);

	const tr::rgba8 outline_color{tint.r, tint.g, tint.b, u8(tint.a * opacity)};
	const tr::gfx::simple_color_mesh_ref outline{g_renderer->basic.new_color_outline(layer::UI, 4)};
	tr::fill_rectangle_outline_vertices(outline.positions, {tl + OUTLINE_THICKNESS / 2, size() - OUTLINE_THICKNESS}, OUTLINE_THICKNESS);
	std::ranges::fill(outline.colors, outline_color);
}