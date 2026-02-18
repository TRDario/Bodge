///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements arrow_widget from ui/widget.hpp.                                                                                           //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Left arrow mesh positions.
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

// Right arrow mesh positions.
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

// Arrow mesh colors.
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

/////////////////////////////////////////////////////////////// ARROW WIDGET //////////////////////////////////////////////////////////////

arrow_widget::arrow_widget(tweened_position pos, tr::valign alignment, ticks unhide_time, arrow_type type, status_callback status_cb,
						   action_callback action_cb)
	: widget{pos, alignment | (type == arrow_type::RIGHT ? tr::halign::RIGHT : tr::halign::LEFT), unhide_time, NO_TOOLTIP}
	, m_scb{std::move(status_cb)}
	, m_acb{std::move(action_cb)}
	, m_tint{m_scb() ? GRAY : DISABLED_GRAY}
	, m_type{type}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
{
}

glm::vec2 arrow_widget::size() const
{
	return {30, 48};
}

void arrow_widget::add_to_renderer()
{
	tr::rgba8 color;
	if (m_action_animation_timer.active()) {
		color = std::fmod(m_action_animation_timer.elapsed_ratio(), 1.0f / 3.0f) >= 1.0f / 6.0f ? WHITE : DISABLED_GRAY;
	}
	else {
		color = m_tint;
	}
	color.a *= opacity();

	constexpr usize INDICES{tr::polygon_outline_indices(5) + tr::polygon_indices(5)};
	const glm::vec2 tl{this->tl()};
	const std::array<glm::vec2, 15>& positions{m_type == arrow_type::RIGHT ? RIGHT_ARROW_POSITIONS : LEFT_ARROW_POSITIONS};
	const tr::gfx::color_mesh_ref arrow{g_renderer->basic.new_color_mesh(layer::UI, 15, INDICES)};

	auto indices_it{arrow.indices.begin()};
	indices_it = tr::fill_convex_polygon_outline_indices(indices_it, 5, arrow.base_index);
	indices_it = tr::fill_convex_polygon_indices(indices_it, 5, arrow.base_index + 10);

	std::ranges::copy(positions | std::views::transform([&](glm::vec2 p) { return p + tl; }), arrow.positions.begin());
	std::ranges::copy(ARROW_COLORS | std::views::transform([tint = color](tr::rgba8 color) -> tr::rgba8 {
						  return {u8(color.r * tr::norm_cast<float>(tint.r)), u8(color.g * tr::norm_cast<float>(tint.g)),
								  u8(color.b * tr::norm_cast<float>(tint.b)), u8(color.a * tr::norm_cast<float>(tint.a))};
					  }),
					  arrow.colors.begin());
}

void arrow_widget::tick()
{
	widget::tick();
	m_tint.tick();

	if (interactible()) {
		if (!m_held && !m_hovered && !m_selected && m_tint.done() && !m_action_animation_timer.active() && m_tint != GRAY) {
			m_tint.change(GRAY, 0.1_s);
		}
		else if (m_tint.done() && (m_hovered || m_selected) && !m_held && !m_action_animation_timer.active()) {
			m_tint.change(tr::color_cast<tr::rgba8>(tr::hsv{float(g_settings.primary_hue), 0.2f, 1.0f}), 4_s, cycle::YES);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if ((m_tint.done() && !m_action_animation_timer.active() && m_tint != DISABLED_GRAY) || m_tint.cycling()) {
			m_tint.change(DISABLED_GRAY, 0.1_s);
		}
	}

	m_action_animation_timer.tick();
}

bool arrow_widget::interactible() const
{
	return m_scb();
}

void arrow_widget::on_action()
{
	m_acb();
	m_action_animation_timer.start();
	m_tint = WHITE;
	g_audio.play_sound(sound::CONFIRM, 0.5f, 0.0f, g_rng.generate(0.9f, 1.1f));
}

void arrow_widget::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_tint.change(WHITE, 0.1_s);
			g_audio.play_sound(sound::HOVER, 0.15f, 0.0f, g_rng.generate(0.9f, 1.1f));
		}
	}
}

void arrow_widget::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected && !m_action_animation_timer.active()) {
			m_tint.change(GRAY, 0.1_s);
		}
	}
}

void arrow_widget::on_held()
{
	if (interactible()) {
		m_held = true;
		if (!m_action_animation_timer.active()) {
			m_tint = HELD_GRAY;
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
			m_tint.change(WHITE, 0.1_s);
		}
	}
}

void arrow_widget::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered && !m_action_animation_timer.active()) {
			m_tint.change(GRAY, 0.1_s);
		}
	}
}