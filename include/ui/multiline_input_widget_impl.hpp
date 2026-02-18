///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements multiline_input_widget from ui/widget.hpp.                                                                                 //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../renderer.hpp"
#include "widget.hpp"

////////////////////////////////////////////////////////// MULTILINE INPUT WIDGET /////////////////////////////////////////////////////////

template <usize S>
multiline_input_widget<S>::multiline_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, float width, u8 max_lines,
												  float font_size, status_callback status_cb)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  NO_TOOLTIP,
				  buffer_text_callback{this->buffer},
				  font::LANGUAGE,
				  tr::sys::ttf_style::NORMAL,
				  font_size,
				  int(width)}
	, m_scb{std::move(status_cb)}
	, m_size{width, g_text_engine.line_skip(font::LANGUAGE, font_size) * max_lines + 4}
	, m_max_lines{max_lines}
	, m_tint{m_scb() ? GRAY : DISABLED_GRAY}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
{
}

template <usize S> glm::vec2 multiline_input_widget<S>::size() const
{
	return m_size;
}

template <usize S> void multiline_input_widget<S>::add_to_renderer()
{
	const tr::gfx::simple_color_mesh_ref fill{g_renderer->basic.new_color_fan(layer::UI, 4)};
	tr::fill_rectangle_vertices(fill.positions, {tl() + 2.0f, size() - 4.0f});
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, u8(160 * opacity())});

	tr::rgba8 color{m_tint};
	if (this->buffer.empty()) {
		tr::rgba8 text_color{color};
		text_color.r /= 2;
		text_color.g /= 2;
		text_color.b /= 2;
		text_widget::add_to_renderer_raw(text_color);
	}
	else {
		text_widget::add_to_renderer_raw(m_tint);
	}
	color.a *= opacity();

	const tr::gfx::simple_color_mesh_ref outline{g_renderer->basic.new_color_outline(layer::UI, 4)};
	tr::fill_rectangle_outline_vertices(outline.positions, {tl() + 1.0f, size() - 2.0f}, 2.0f);
	std::ranges::fill(outline.colors, color);
}

template <usize S> void multiline_input_widget<S>::tick()
{
	text_widget::tick();
	m_tint.tick();

	if (interactible()) {
		if (interactible() && !(m_held || m_hovered || m_selected) && m_tint.done() && m_tint != GRAY) {
			m_tint.change(GRAY, 0.1_s);
		}
		else if (m_tint.done() && (m_hovered || m_selected) && !m_held) {
			m_tint.change(tr::color_cast<tr::rgba8>(tr::hsv{float(g_settings.primary_hue), 0.2f, 1.0f}), 4_s, cycle::YES);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if ((m_tint.done() && m_tint != DISABLED_GRAY) || m_tint.cycling()) {
			m_tint.change(DISABLED_GRAY, 0.1_s);
		}
	}
}

template <usize S> bool multiline_input_widget<S>::interactible() const
{
	return m_scb();
}

template <usize S> bool multiline_input_widget<S>::writable() const
{
	return true;
}

template <usize S> void multiline_input_widget<S>::on_action()
{
	m_tint.change(WHITE, 0.1_s);
}

template <usize S> void multiline_input_widget<S>::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_tint.change(WHITE, 0.1_s);
			g_audio.play_sound(sound::HOVER, 0.15f, 0.0f, g_rng.generate(0.9f, 1.1f));
		}
	}
}

template <usize S> void multiline_input_widget<S>::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected) {
			m_tint.change(GRAY, 0.1_s);
		}
	}
}

template <usize S> void multiline_input_widget<S>::on_held()
{
	if (interactible()) {
		m_held = true;
		m_tint = HELD_GRAY;
	}
}

template <usize S> void multiline_input_widget<S>::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

template <usize S> void multiline_input_widget<S>::on_selected()
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

template <usize S> void multiline_input_widget<S>::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered) {
			m_tint.change(GRAY, 0.1_s);
		}
	}
}

template <usize S> void multiline_input_widget<S>::on_write(std::string_view input)
{
	if (tr::utf8::length(this->buffer) + tr::utf8::length(input) <= S) {
		this->buffer.append(input);
		if (g_text_engine.count_lines(this->buffer, font::LANGUAGE, tr::sys::ttf_style::NORMAL, m_font_size, m_font_size / 12, m_size.x) >
			m_max_lines) {
			this->buffer.resize(this->buffer.size() - input.size());
		}
		else {
			g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
		}
	}
}

template <usize S> void multiline_input_widget<S>::on_enter()
{
	if (tr::utf8::length(this->buffer) < S && g_text_engine.count_lines(this->buffer, font::LANGUAGE, tr::sys::ttf_style::NORMAL,
																		m_font_size, m_font_size / 12, m_size.x) < m_max_lines) {
		this->buffer.append('\n');
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <usize S> void multiline_input_widget<S>::on_erase()
{
	if (!this->buffer.empty()) {
		tr::utf8::pop_back(this->buffer);
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <usize S> void multiline_input_widget<S>::on_clear()
{
	this->buffer.clear();
	g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
}

template <usize S> void multiline_input_widget<S>::on_copy()
{
	tr::sys::set_clipboard_text(std::string{this->buffer}.c_str());
}

template <usize S> void multiline_input_widget<S>::on_paste()
{
	try {
		if (!tr::sys::clipboard_empty()) {
			std::string pasted{tr::sys::clipboard_text()};
			tr::static_string<S * 4> copy{this->buffer};
			const usize buffer_length{tr::utf8::length(this->buffer)};
			copy += (buffer_length + tr::utf8::length(pasted) > S)
						? std::string_view{pasted.begin(), tr::utf8::next(pasted.begin(), S - buffer_length)}
						: pasted;
			// Replace this with a smarter solution eventually, maybe.
			if (g_text_engine.count_lines(copy, font::LANGUAGE, tr::sys::ttf_style::NORMAL, m_font_size, m_font_size / 12, m_size.x) <=
				m_max_lines) {
				this->buffer = copy;
			}
			g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
		}
	}
	catch (...) {
	}
}