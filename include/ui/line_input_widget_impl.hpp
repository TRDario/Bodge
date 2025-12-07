#pragma once
#include "widget.hpp"

template <usize S>
line_input_widget<S>::line_input_widget(tweened_position pos, tr::align alignment, ticks unhide_time, tr::sys::ttf_style style,
										float font_size, status_callback status_cb, action_callback enter_cb, std::string_view initial_text)
	: input_buffer<S>{initial_text}
	, text_widget{pos,
				  alignment,
				  unhide_time,
				  NO_TOOLTIP,
				  buffer_text_callback{this->buffer},
				  font::LANGUAGE,
				  style,
				  font_size,
				  tr::sys::UNLIMITED_WIDTH}
	, m_scb{std::move(status_cb)}
	, m_enter_cb{std::move(enter_cb)}
	, m_tint{m_scb() ? GRAY : DISABLED_GRAY}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
{
}

template <usize S> void line_input_widget<S>::add_to_renderer()
{
	if (this->buffer.empty()) {
		tr::rgba8 color{m_tint};
		color.r /= 2;
		color.g /= 2;
		color.b /= 2;
		text_widget::add_to_renderer_raw(color);
	}
	else {
		text_widget::add_to_renderer_raw(m_tint);
	}
}

template <usize S> void line_input_widget<S>::tick()
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

template <usize S> bool line_input_widget<S>::interactible() const
{
	return m_scb();
}

template <usize S> bool line_input_widget<S>::writable() const
{
	return true;
}

template <usize S> void line_input_widget<S>::on_action()
{
	m_tint.change(WHITE, 0.1_s);
}

template <usize S> void line_input_widget<S>::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_tint.change(WHITE, 0.1_s);
			g_audio.play_sound(sound::HOVER, 0.15f, 0.0f, g_rng.generate(0.9f, 1.1f));
		}
	}
}

template <usize S> void line_input_widget<S>::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected) {
			m_tint.change(GRAY, 0.1_s);
		}
	}
}

template <usize S> void line_input_widget<S>::on_held()
{
	if (interactible()) {
		m_held = true;
		m_tint = HELD_GRAY;
	}
}

template <usize S> void line_input_widget<S>::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

template <usize S> void line_input_widget<S>::on_selected()
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

template <usize S> void line_input_widget<S>::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered) {
			m_tint.change(GRAY, 0.1_s);
		}
	}
}

template <usize S> void line_input_widget<S>::on_write(std::string_view input)
{
	if (tr::utf8::length(this->buffer) + tr::utf8::length(input) <= S) {
		this->buffer.append(input);
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <usize S> void line_input_widget<S>::on_enter()
{
	m_enter_cb();
}

template <usize S> void line_input_widget<S>::on_erase()
{
	if (!this->buffer.empty()) {
		tr::utf8::pop_back(this->buffer);
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}

template <usize S> void line_input_widget<S>::on_clear()
{
	this->buffer.clear();
	g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
}

template <usize S> void line_input_widget<S>::on_copy()
{
	tr::sys::set_clipboard_text(std::string{this->buffer});
}

template <usize S> void line_input_widget<S>::on_paste()
{
	const usize buffer_length{tr::utf8::length(this->buffer)};
	if (!tr::sys::clipboard_empty() && buffer_length < S) {
		std::string pasted{tr::sys::clipboard_text()};
		std::erase(pasted, '\n');
		this->buffer += (buffer_length + tr::utf8::length(pasted) > S)
							? std::string_view{pasted.begin(), tr::utf8::next(pasted.begin(), S - buffer_length)}
							: pasted;
		g_audio.play_sound(sound::TYPE, 0.2f, 0.0f, g_rng.generate(0.75f, 1.25f));
	}
}