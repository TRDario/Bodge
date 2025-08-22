#include "../../include/ui/widget.hpp"

//

constexpr std::array<glm::vec2, 15> LEFT_ARROW_POSITIONS{{
	{30, 0},
	{16, 0},
	{0, 24},
	{16, 48},
	{30, 48},
	{26, 4},
	{18.30940108, 4},
	{4.61880215, 24},
	{18.30940108, 44},
	{26, 44},
	{26, 4},
	{18.30940108, 4},
	{4.61880215, 24},
	{18.30940108, 44},
	{26, 44},
}};

constexpr std::array<glm::vec2, 15> RIGHT_ARROW_POSITIONS{{
	{0, 0},
	{14, 0},
	{30, 24},
	{14, 48},
	{0, 48},
	{4, 4},
	{11.69059892, 4},
	{25.38119785, 24},
	{11.69059892, 44},
	{4, 44},
	{4, 4},
	{11.69059892, 4},
	{25.38119785, 24},
	{11.69059892, 44},
	{4, 44},
}};

constexpr std::array<tr::rgba8, 15> ARROW_COLORS{{
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{255, 255, 255, 255},
	{255, 255, 255, 255},
	{213, 213, 213, 255},
	{192, 192, 192, 255},
	{192, 192, 192, 255},
}};

//

arrow_widget::arrow_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, bool right_arrow, status_callback status_cb,
						   action_callback action_cb)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP, false}
	, m_scb{std::move(status_cb)}
	, m_acb{std::move(action_cb)}
	, m_interp{m_scb() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8}
	, m_right{right_arrow}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
	, m_action_left{0}
{
}

glm::vec2 arrow_widget::size() const
{
	return {30, 48};
}

void arrow_widget::add_to_renderer()
{
	tr::rgba8 color;
	if (m_action_left > 0) {
		color = m_action_left % 0.12_s >= 0.06_s ? "FFFFFF"_rgba8 : "505050A0"_rgba8;
	}
	else {
		color = m_interp;
	}
	color.a *= opacity();

	const glm::vec2 tl{this->tl()};
	const std::array<glm::vec2, 15>& positions{m_right ? RIGHT_ARROW_POSITIONS : LEFT_ARROW_POSITIONS};
	const tr::gfx::color_mesh_ref arrow{tr::gfx::renderer_2d::new_color_mesh(layer::UI, 15, tr::poly_outline_idx(5) + tr::poly_idx(5))};
	tr::fill_poly_outline_idx(arrow.indices.begin(), 5, arrow.base_index);
	tr::fill_poly_idx(arrow.indices.begin() + tr::poly_outline_idx(5), 5, arrow.base_index + 10);
	std::ranges::copy(positions | std::views::transform([&](glm::vec2 p) { return p + tl; }), arrow.positions.begin());
	std::ranges::copy(ARROW_COLORS | std::views::transform([&](tr::rgba8 c) -> tr::rgba8 {
						  return {std::uint8_t(c.r * tr::norm_cast<float>(color.r)), std::uint8_t(c.g * tr::norm_cast<float>(color.g)),
								  std::uint8_t(c.b * tr::norm_cast<float>(color.b)), std::uint8_t(c.a * tr::norm_cast<float>(color.a))};
					  }),
					  arrow.colors.begin());
}

void arrow_widget::update()
{
	widget::update();
	m_interp.update();

	if (interactible()) {
		if (!m_held && !m_hovered && !m_selected && m_interp.done() && m_action_left == 0 && m_interp != "A0A0A0A0"_rgba8) {
			m_interp.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
		else if (m_interp.done() && (m_hovered || m_selected) && !m_held && m_action_left == 0) {
			m_interp.change(tween::CYCLE, tr::color_cast<tr::rgba8>(tr::hsv{float(engine::settings.primary_hue), 0.2f, 1.0f}), 4_s);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if (m_interp.done() && m_action_left == 0 && m_interp != "505050A0"_rgba8) {
			m_interp.change(tween::LERP, "505050A0"_rgba8, 0.1_s);
		}
	}

	if (m_action_left > 0) {
		--m_action_left;
	}
}

bool arrow_widget::interactible() const
{
	return m_scb();
}

void arrow_widget::on_action()
{
	m_acb();
	m_action_left = 0.36_s;
	m_interp = "FFFFFF"_rgba8;
	engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

void arrow_widget::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(tween::LERP, "FFFFFF"_rgba8, 0.1_s);
			engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

void arrow_widget::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected && m_action_left == 0) {
			m_interp.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

void arrow_widget::on_held()
{
	if (interactible()) {
		m_held = true;
		if (m_action_left == 0) {
			m_interp = "202020"_rgba8;
		}
	}
}

void arrow_widget::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

void arrow_widget::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_interp.change(tween::LERP, "FFFFFF"_rgba8, 0.1_s);
		}
	}
}

void arrow_widget::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered && m_action_left == 0) {
			m_interp.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}