#include "../../include/ui/widget.hpp"

text_button_widget::text_button_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb,
									   text_callback text_cb, font font, float font_size, status_callback status_cb,
									   action_callback action_cb, sound action_sound)
	: text_widget{pos,  alignment,          unhide_time, std::move(tooltip_cb),   false, std::move(text_cb),
				  font, text_style::NORMAL, font_size,   tr::sys::UNLIMITED_WIDTH}
	, m_scb{std::move(status_cb)}
	, m_acb{std::move(action_cb)}
	, m_action_sound{action_sound}
	, m_interp{m_scb() ? GRAY : DISABLED_GRAY}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
	, m_action_left{0}
{
}

void text_button_widget::update()
{
	text_widget::update();
	m_interp.update();

	if (interactible()) {
		if (!m_held && !m_hovered && !m_selected && m_interp.done() && m_action_left == 0 && m_interp != GRAY) {
			m_interp.change(tween::LERP, GRAY, 0.1_s);
		}
		else if (m_interp.done() && (m_hovered || m_selected) && !m_held && m_action_left == 0) {
			m_interp.change(tween::CYCLE, tr::color_cast<tr::rgba8>(tr::hsv{float(g_settings.primary_hue), 0.2f, 1.0f}), 4_s);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if ((m_interp.done() && m_action_left == 0 && m_interp != DISABLED_GRAY) || m_interp.cycling()) {
			m_interp.change(tween::LERP, DISABLED_GRAY, 0.1_s);
		}
	}

	if (m_action_left > 0) {
		--m_action_left;
	}
}

void text_button_widget::add_to_renderer()
{
	if (m_action_left > 0) {
		text_widget::add_to_renderer_raw(m_action_left % 0.12_s >= 0.06_s ? WHITE : DISABLED_GRAY);
	}
	else {
		text_widget::add_to_renderer_raw(m_interp);
	}
}

bool text_button_widget::interactible() const
{
	return m_scb();
}

void text_button_widget::on_action()
{
	m_acb();
	m_action_left = 0.36_s;
	m_interp = WHITE;
	g_audio.play_sound(m_action_sound, 0.5f, 0.0f, g_rng.generate(0.9f, 1.1f));
}

void text_button_widget::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(tween::LERP, WHITE, 0.1_s);
			g_audio.play_sound(sound::HOVER, 0.15f, 0.0f, g_rng.generate(0.9f, 1.1f));
		}
	}
}

void text_button_widget::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected && m_action_left == 0) {
			m_interp.change(tween::LERP, GRAY, 0.1_s);
		}
	}
}

void text_button_widget::on_held()
{
	if (interactible()) {
		m_held = true;
		if (m_action_left == 0) {
			m_interp = HELD_GRAY;
		}
	}
}

void text_button_widget::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

void text_button_widget::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_interp.change(tween::LERP, WHITE, 0.1_s);
		}
	}
}

void text_button_widget::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered && m_action_left == 0) {
			m_interp.change(tween::LERP, GRAY, 0.1_s);
		}
	}
}