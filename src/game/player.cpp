#include "../../include/game/player.hpp"
#include "../../include/graphics.hpp"
#include "../../include/settings.hpp"

//

inline constexpr ticks INVULNERABILITY_TIME{2_s};
inline constexpr ticks INITIAL_INVULNERABILITY_TIME{2.5_s};

//

player::player(const player_settings& settings)
	: m_hitbox{{500, 500}, settings.hitbox_radius}
	, m_trail{m_hitbox.c}
	, m_inertia{settings.inertia_factor}
	, m_iframes_left{INITIAL_INVULNERABILITY_TIME}
{
}

//

bool player::invincible() const
{
	return m_iframes_left > 0;
}

const tr::circle& player::hitbox() const
{
	return m_hitbox;
}

//

void player::hit()
{
	m_iframes_left = INVULNERABILITY_TIME;
}

void player::kill()
{
	for (usize i = 0; i < m_fragments.size(); ++i) {
		const tr::angle th{60_deg * i + 30_deg};
		const glm::vec2 vel{tr::magth(engine::rng.generate(100.0f, 400.0f), engine::rng.generate(th - 30_deg, th + 30_deg))};
		const tr::angle ang_vel{engine::rng.generate(360_deg, 720_deg) * (engine::rng.generate_bool() ? 1 : -1)};
		m_fragments[i] = {m_hitbox.c + tr::magth(m_hitbox.r, th), vel, th + 90_deg, ang_vel};
	}
}

void player::update(glm::vec2 target)
{
	if (m_iframes_left > 0) {
		--m_iframes_left;
	}

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

void player::add_to_renderer_alive(ticks time_since_start) const
{
	constexpr float PI{std::numbers::pi_v<float>};

	const tr::rgb8 tint{color_cast<tr::rgb8>(tr::hsv{float(engine::settings.primary_hue), 1, 1})};
	const u8 opacity{tr::norm_cast<u8>(std::abs(std::cos(m_iframes_left * PI * 8 / INVULNERABILITY_TIME)))};
	const tr::angle rotation{tr::degs(270.0f * time_since_start / SECOND_TICKS)};
	const float size_offset{3.0f * std::sin(PI * time_since_start / SECOND_TICKS)};
	const float size{m_hitbox.r + 6 + size_offset};

	if (opacity != 0) {
		add_fill_to_renderer(opacity, rotation, size);
		add_outline_to_renderer(tint, opacity, rotation, size);
		add_trail_to_renderer(tint, opacity, rotation, size);
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
	const tr::gfx::simple_color_mesh_ref fill{tr::gfx::renderer_2d::new_color_fan(layer::PLAYER, 6)};
	tr::fill_poly_vtx(fill.positions, 6, {m_hitbox.c, size}, rotation);
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, opacity});
}

void player::add_outline_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const
{
	const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::PLAYER, 6)};
	tr::fill_poly_outline_vtx(outline.positions, 6, {m_hitbox.c, size}, rotation, 4.0f);
	std::ranges::fill(outline.colors | std::views::take(6), tr::rgba8{tint, opacity});
	std::ranges::fill(outline.colors | std::views::drop(6), tr::rgba8{0, 0, 0, opacity});
}

void player::add_trail_to_renderer(tr::rgb8 tint, u8 opacity, tr::angle rotation, float size) const
{
	constexpr usize VERTICES{6 * (trail::SIZE + 1)};
	constexpr usize INDICES{tr::poly_outline_idx(6) * trail::SIZE};

	tr::gfx::color_mesh_ref trail_mesh{tr::gfx::renderer_2d::new_color_mesh(layer::PLAYER_TRAIL, VERTICES, INDICES)};
	tr::fill_poly_vtx(trail_mesh.positions | std::views::take(6), 6, {m_hitbox.c, size}, rotation);
	std::ranges::fill(trail_mesh.colors, tr::rgba8{tint, opacity});

	std::vector<u16>::iterator indices_it{trail_mesh.indices.begin()};
	for (usize i = 0; i < m_trail.SIZE; ++i) {
		const float trail_fade{float(m_trail.SIZE - i) / m_trail.SIZE};
		const float trail_size{size * trail_fade};
		const u8 trail_opacity{u8(opacity / 3.0f * trail_fade)};

		tr::fill_poly_vtx(trail_mesh.positions | std::views::drop(6 * (i + 1)), 6, {m_trail[i], trail_size}, rotation);
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

void player::add_death_wave_to_renderer(ticks time_since_game_over) const
{
	const float t{(time_since_game_over + 1) / float(0.5_s)};
	const float scale{std::sqrt(t) * 200};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{float(engine::settings.primary_hue), 1, 1})};
	const u8 opacity{tr::norm_cast<u8>(0.5f * std::sqrt(1 - t))};

	const usize indices{tr::smooth_poly_vtx(scale, engine::render_scale()) + 2};
	const tr::gfx::simple_color_mesh_ref fan{tr::gfx::renderer_2d::new_color_fan(layer::PLAYER_TRAIL, indices)};
	fan.positions[0] = m_hitbox.c;
	fan.colors[0] = {color, 0};
	tr::fill_poly_vtx(fan.positions | std::views::drop(1), fan.positions.size() - 2, {m_hitbox.c, scale});
	fan.positions.back() = fan.positions[1];
	std::ranges::fill(fan.colors | std::views::drop(1), tr::rgba8{color, opacity});
}

void player::add_death_fragments_to_renderer(ticks time_since_game_over) const
{
	const float t{(time_since_game_over + 1) / float(0.5_s)};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{float(engine::settings.primary_hue), 1, 1})};
	const u8 opacity{tr::norm_cast<u8>(std::sqrt(1 - t))};
	const float length{2 * m_hitbox.r * tr::degs(30.0f).tan()};

	for (const fragment& fragment : m_fragments) {
		const tr::gfx::simple_color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_fan(layer::PLAYER, 4)};
		tr::fill_rotated_rect_vtx(mesh.positions, fragment.pos, {length / 2, 2}, {length, 4}, fragment.rot);
		std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
	}
}