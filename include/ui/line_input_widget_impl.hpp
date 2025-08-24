#pragma once
#include "widget.hpp"

template <usize S>
line_input_widget<S>::line_input_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, tr::system::ttf_style style,
										float font_size, status_callback status_cb, action_callback enter_cb, std::string_view initial_text)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  NO_TOOLTIP,
				  true,
				  [this] { return buffer.empty() ? std::string{engine::loc["empty"]} : std::string{buffer}; },
				  font::LANGUAGE,
				  style,
				  font_size,
				  tr::system::UNLIMITED_WIDTH}
	, buffer{initial_text}
	, m_scb{std::move(status_cb)}
	, m_enter_cb{std::move(enter_cb)}
	, m_interp{m_scb() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
{
}

template <usize S> void line_input_widget<S>::add_to_renderer()
{
	if (buffer.empty()) {
		tr::rgba8 color{m_interp};
		color.r /= 2;
		color.g /= 2;
		color.b /= 2;
		text_widget::add_to_renderer_raw(color);
	}
	else {
		text_widget::add_to_renderer_raw(m_interp);
	}
}

template <usize S> void line_input_widget<S>::update()
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

template <usize S> bool line_input_widget<S>::interactible() const
{
	return m_scb();
}

template <usize S> void line_input_widget<S>::on_action()
{
	m_interp.change(tween::LERP, "FFFFFF"_rgba8, 0.1_s);
}

template <usize S> void line_input_widget<S>::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(tween::LERP, "FFFFFF"_rgba8, 0.1_s);
			engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

template <usize S> void line_input_widget<S>::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected) {
			m_interp.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

template <usize S> void line_input_widget<S>::on_held()
{
	if (interactible()) {
		m_held = true;
		m_interp = "202020"_rgba8;
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
			m_interp.change(tween::LERP, "FFFFFF"_rgba8, 0.1_s);
		}
		else {
			engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

template <usize S> void line_input_widget<S>::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered) {
			m_interp.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

template <usize S> void line_input_widget<S>::on_write(std::string_view input)
{
	if (tr::utf8::length(buffer) + tr::utf8::length(input) <= S) {
		buffer.append(input);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <usize S> void line_input_widget<S>::on_enter()
{
	m_enter_cb();
}

template <usize S> void line_input_widget<S>::on_erase()
{
	if (!buffer.empty()) {
		tr::utf8::pop_back(buffer);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <usize S> void line_input_widget<S>::on_clear()
{
	buffer.clear();
	engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
}

template <usize S> void line_input_widget<S>::on_copy()
{
	tr::system::set_clipboard_text(std::string{buffer});
}

template <usize S> void line_input_widget<S>::on_paste()
{
	const usize buffer_length{tr::utf8::length(buffer)};
	if (!tr::system::clipboard_empty() && buffer_length < S) {
		std::string pasted{tr::system::clipboard_text()};
		std::erase(pasted, '\n');
		buffer += (buffer_length + tr::utf8::length(pasted) > S)
					  ? std::string_view{pasted.begin(), tr::utf8::next(pasted.begin(), S - buffer_length)}
					  : pasted;
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}