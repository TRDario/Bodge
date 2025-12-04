#include "../../include/game/player.hpp"
#include "../../include/graphics/graphics.hpp"
#include "../../include/settings.hpp"

//

player::player(const player_settings& settings)
	: m_hitbox{{500, 500}, settings.hitbox_radius}, m_trail{m_hitbox.c}, m_inertia{settings.inertia_factor}
{
	m_invincibility_timer.start();
}

//

bool player::invincible() const
{
	return m_invincibility_timer.active();
}

const tr::circle& player::hitbox() const
{
	return m_hitbox;
}

//

void player::hit()
{
	m_invincibility_timer.start();
}

void player::kill()
{
	for (usize i = 0; i < m_fragments.size(); ++i) {
		const tr::angle th{60_deg * i + 30_deg};
		const glm::vec2 vel{tr::magth(g_rng.generate(100.0f, 400.0f), g_rng.generate(th - 30_deg, th + 30_deg))};
		const tr::angle ang_vel{g_rng.generate(360_deg, 720_deg) * (g_rng.generate_bool() ? 1 : -1)};
		m_fragments[i] = {m_hitbox.c + tr::magth(m_hitbox.r, th), vel, th + 90_deg, ang_vel};
	}
}

void player::update(glm::vec2 target)
{
	m_invincibility_timer.update();

	target = glm::clamp(target, glm::vec2{FIELD_MIN + m_hitbox.r}, glm::vec2{FIELD_MAX - m_hitbox.r});

	m_trail.push(m_hitbox.c);
	if (m_inertia == 0) {
		m_hitbox.c = target;
	}
	else {
		m_hitbox.c = m_hitbox.c + (target - m_hitbox.c) / 1_sf / m_inertia;
	}
}

void player::update_fragments()
{
	for (fragment& fragment : m_fragments) {
		fragment.update();
	}
}

//

void player::add_to_renderer_alive(ticks time_since_start, const decrementing_timer<0.1_s>& style_cooldown_timer) const
{
	const tr::rgb8 tint{color_cast<tr::rgb8>(tr::hsv{float(g_settings.primary_hue), 1, 1})};
	const u8 opacity{tr::norm_cast<u8>(std::abs(tr::turns(4.0f * m_invincibility_timer.elapsed_ratio()).cos()))};
	const tr::angle rotation{270_deg * time_since_start / 1_s};
	const float size_offset{3.0f * tr::turns(time_since_start / 2_sf).sin()};
	const float size{m_hitbox.r + 6 + size_offset};

	if (opacity != 0) {
		add_fill_to_renderer(opacity, rotation, size);
		add_outline_to_renderer(tint, opacity, rotation, size);
		add_trail_to_renderer(tint, opacity, rotation, size);
		add_style_wave_to_renderer(tint, style_cooldown_timer);
	}
}

void player::add_to_renderer_dead(ticks time_since_game_over) const
{
	if (time_since_game_over < 0.5_s) {
		add_death_wave_to_renderer(time_since_game_over);
		add_death_fragments_to_renderer(time_since_game_over);
	}
}

//

void player::add_fill_to_renderer(u8 opacity, tr::angle rotation, float size) const
{
	const tr::gfx::simple_color_mesh_ref fill{g_graphics->basic_renderer.new_color_fan(layer::PLAYER, 6)};
	tr::fill_regular_polygon_vertices(fill.positions, {m_hitbox.c, size}, rotation);
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, opacity});
}

void player::add_outline_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const
{
	const tr::gfx::simple_color_mesh_ref outline{g_graphics->basic_renderer.new_color_outline(layer::PLAYER, 6)};
	tr::fill_regular_polygon_outline_vertices(outline.positions, {m_hitbox.c, size}, rotation, 4.0f);
	std::fill_n(outline.colors.begin(), 6, tr::rgba8{tint, opacity});
	std::fill_n(outline.colors.begin() + 6, 6, tr::rgba8{0, 0, 0, opacity});
}

void player::add_trail_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const
{
	constexpr usize VERTICES{6 * (trail::size() + 1)};
	constexpr usize INDICES{tr::polygon_outline_indices(6) * trail::size()};

	tr::gfx::color_mesh_ref trail_mesh{g_graphics->basic_renderer.new_color_mesh(layer::PLAYER_TRAIL, VERTICES, INDICES)};
	tr::fill_regular_polygon_vertices(trail_mesh.positions.begin(), 6, {m_hitbox.c, size}, rotation);
	std::ranges::fill(trail_mesh.colors, tr::rgba8{tint, opacity});

	std::vector<u16>::iterator indices_it{trail_mesh.indices.begin()};
	for (usize i = 0; i < m_trail.size(); ++i) {
		const float trail_fade{float(m_trail.size() - i) / m_trail.size()};
		const float trail_size{size * trail_fade};
		const u8 trail_opacity{u8(opacity / 3.0f * trail_fade)};

		tr::fill_regular_polygon_vertices(trail_mesh.positions.begin() + 6 * (i + 1), 6, {m_trail[i], trail_size}, rotation);
		for (int j = 0; j < 6; ++j) {
			trail_mesh.colors[6 * (i + 1) + j].a = trail_opacity;
			*indices_it++ = u16(trail_mesh.base_index + 6 * (i + 1) + j);
			*indices_it++ = u16(trail_mesh.base_index + 6 * (i + 1) + ((j + 1) % 6));
			*indices_it++ = u16(trail_mesh.base_index + 6 * i + ((j + 1) % 6));
			*indices_it++ = u16(trail_mesh.base_index + 6 * (i + 1) + j);
			*indices_it++ = u16(trail_mesh.base_index + 6 * i + ((j + 1) % 6));
			*indices_it++ = u16(trail_mesh.base_index + 6 * i + j);
		}
	}
}

void player::add_style_wave_to_renderer(tr::rgb8 tint, const decrementing_timer<0.1_s>& timer) const
{
	if (!timer.active()) {
		return;
	}

	const float t{timer.elapsed_ratio()};
	const float scale{m_hitbox.r + 10 + std::pow(t, 2.0f) * 40};
	const usize vertices{tr::smooth_polygon_vertices(scale * g_graphics->render_scale())};
	const u8 opacity{tr::norm_cast<u8>(std::sqrt(1 - t) * 0.75f)};

	const tr::gfx::simple_color_mesh_ref mesh{g_graphics->basic_renderer.new_color_outline(layer::PLAYER, vertices)};
	tr::fill_circle_outline_vertices(mesh.positions, {m_hitbox.c, scale}, 2);
	std::ranges::fill(mesh.colors, tr::rgba8{tint, opacity});
}

void player::add_death_wave_to_renderer(ticks time_since_game_over) const
{
	const float t{(time_since_game_over + 1) / 0.5_sf};
	const float scale{std::sqrt(t) * 200};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{float(g_settings.primary_hue), 1, 1})};
	const u8 opacity{tr::norm_cast<u8>(0.5f * std::sqrt(1 - t))};

	const usize indices{tr::smooth_polygon_vertices(scale * g_graphics->render_scale()) + 2};
	const tr::gfx::simple_color_mesh_ref fan{g_graphics->basic_renderer.new_color_fan(layer::PLAYER_TRAIL, indices)};

	fan.positions[0] = m_hitbox.c;
	tr::fill_circle_vertices(fan.positions.begin() + 1, fan.positions.size() - 2, {m_hitbox.c, scale});
	fan.positions.back() = fan.positions[1];

	fan.colors[0] = {color, 0};
	std::fill(fan.colors.begin() + 1, fan.colors.end(), tr::rgba8{color, opacity});
}

void player::add_death_fragments_to_renderer(ticks time_since_game_over) const
{
	const float t{(time_since_game_over + 1) / 0.5_sf};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{float(g_settings.primary_hue), 1, 1})};
	const u8 opacity{tr::norm_cast<u8>(std::sqrt(1 - t))};
	const float length{2 * m_hitbox.r * (30_deg).tan()};

	for (const fragment& fragment : m_fragments) {
		const tr::gfx::simple_color_mesh_ref mesh{g_graphics->basic_renderer.new_color_fan(layer::PLAYER, 4)};
		tr::fill_rectangle_vertices(mesh.positions, fragment.pos, {length / 2, 2}, {length, 4}, fragment.rot);
		std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
	}
}