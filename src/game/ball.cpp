#include "../../include/game/ball.hpp"
#include "../../include/audio.hpp"
#include "../../include/graphics.hpp"
#include "../../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

inline constexpr ticks BALL_SPAWN_ANIMATION_TIME{1.5_s};
inline constexpr ticks BALL_COLLISION_ANIMATION_TIME{0.1_s};

////////////////////////////////////////////////////////////////// BALL ///////////////////////////////////////////////////////////////////

void play_ball_sound(glm::vec2 pos, float velocity)
{
	const float pan{(pos.x - 500) / 500};
	const float pitch{std::clamp(velocity / 600, 0.75f, 1.25f)};
	g_audio.play_sound(sound::BOUNCE, 0.15f, pan, g_rng.generate(pitch - 0.2f, pitch + 0.2f));
}

//

ball::ball(const tr::circle& hitbox, const glm::vec2& velocity)
	: m_hitbox{hitbox}, m_trail{hitbox.c}, m_velocity{velocity}, m_time_since_spawned{0}, m_time_since_last_collision{0}
{
}

ball::ball(tr::xorshiftr_128p& rng, float size, float velocity)
	: ball{{{rng.generate(FIELD_MIN + size, FIELD_MAX - size), rng.generate(FIELD_MIN + size, FIELD_MAX - size)}, size},
		   rng.generate_vector(velocity)}
{
}

//

bool ball::tangible() const
{
	return m_time_since_spawned >= BALL_SPAWN_ANIMATION_TIME;
}

const tr::circle& ball::hitbox() const
{
	return m_hitbox;
}

const glm::vec2& ball::velocity() const
{
	return m_velocity;
}

bool colliding(const ball& a, const ball& b)
{
	return tr::intersecting(a.hitbox(), b.hitbox()) && glm::dot(a.m_hitbox.c - b.m_hitbox.c, b.m_velocity - a.m_velocity) >= 0;
}

//

void handle_collision(ball& a, ball& b)
{
	const glm::vec2 dist_vec{a.m_hitbox.c - b.m_hitbox.c};
	const glm::vec2 vel_diff{a.m_velocity - b.m_velocity};
	const glm::vec2 dist2{dist_vec.x * dist_vec.x + dist_vec.y * dist_vec.y};
	const float a_mass{a.m_hitbox.r * a.m_hitbox.r};
	const float b_mass{b.m_hitbox.r * b.m_hitbox.r};
	const float total_mass{a_mass + b_mass};

	play_ball_sound(b.m_hitbox.c + dist_vec / 2.0f, std::max(glm::length(a.m_velocity), glm::length(b.m_velocity)));

	a.m_velocity -= ((2 * b_mass / total_mass) * (glm::dot(dist_vec, vel_diff) / dist2) * dist_vec);
	b.m_velocity -= ((2 * a_mass / total_mass) * (glm::dot(-dist_vec, -vel_diff) / dist2) * -dist_vec);

	a.m_time_since_last_collision = 0;
	b.m_time_since_last_collision = 0;
}

void ball::update()
{
	++m_time_since_spawned;
	++m_time_since_last_collision;

	if (tangible()) {
		m_trail.push(m_hitbox.c);

		const glm::vec2 target{m_hitbox.c + m_velocity / 1_sf};
		const glm::vec2 clamped{tr::mirror_repeat(target, glm::vec2{FIELD_MIN + m_hitbox.r}, glm::vec2{FIELD_MAX - m_hitbox.r})};

		if (target == clamped) {
			m_hitbox.c = target;
			return;
		}

		if (clamped.x > target.x) {
			m_velocity.x = std::abs(m_velocity.x);
		}
		else if (clamped.x < target.x) {
			m_velocity.x = -std::abs(m_velocity.x);
		}

		if (clamped.y > target.y) {
			m_velocity.y = std::abs(m_velocity.y);
		}
		else if (clamped.y < target.y) {
			m_velocity.y = -std::abs(m_velocity.y);
		}

		if (clamped != target) {
			play_ball_sound(clamped, glm::length(m_velocity));
		}

		m_hitbox.c = clamped;
		m_time_since_last_collision = 0;
	}
}

//

void ball::add_to_renderer() const
{
	const tr::rgb8 tint{tr::color_cast<tr::rgb8>(tr::hsv{float(g_settings.secondary_hue), 1, 1})};
	const float raw_age_factor{std::min(float(m_time_since_spawned) / BALL_SPAWN_ANIMATION_TIME, 1.0f)};
	const float eased_age_factor{raw_age_factor == 1.0f ? raw_age_factor : 1.0f - std::pow(2.0f, -10.0f * raw_age_factor)};
	const float size{m_hitbox.r * (5 - 4 * eased_age_factor)};
	const usize vertices{tr::smooth_polygon_vertices(size * g_graphics->render_scale())};
	const u8 base_opacity{tr::norm_cast<u8>(raw_age_factor)};
	const float thickness{
		3 + 4 * std::max((float(BALL_COLLISION_ANIMATION_TIME) - m_time_since_last_collision) / BALL_COLLISION_ANIMATION_TIME, 0.0f),
	};

	// Add the ball.
	const tr::gfx::simple_color_mesh_ref fill{g_graphics->basic_renderer.new_color_fan(layer::BALLS, vertices)};
	tr::fill_circle_vertices(fill.positions, {m_hitbox.c - thickness / 2, size});
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, base_opacity});
	const tr::gfx::simple_color_mesh_ref outline{g_graphics->basic_renderer.new_color_outline(layer::BALLS, vertices)};
	tr::fill_circle_outline_vertices(outline.positions, {m_hitbox.c, size}, thickness);
	std::ranges::fill(outline.colors, tr::rgba8{tint, base_opacity});

	// Add the trail.
	if (m_time_since_spawned > BALL_SPAWN_ANIMATION_TIME) {
		usize drawn_trails{2};
		for (usize i = 0; i < m_trail.size() - 1; ++i) {
			const glm::vec2 prev{i == 0 ? m_hitbox.c : m_trail[i - 1]};
			if (!tr::collinear(prev, m_trail[i], m_trail[i + 1])) {
				++drawn_trails;
			}
		}
		const usize trail_vertices{drawn_trails * vertices};
		const usize trail_indices{(drawn_trails - 1) * vertices * 6};

		tr::gfx::color_mesh_ref trail{g_graphics->basic_renderer.new_color_mesh(layer::BALL_TRAILS, trail_vertices, trail_indices)};
		tr::fill_circle_vertices(trail.positions.begin(), vertices, m_hitbox);
		std::ranges::fill(trail.colors | std::views::take(vertices), tr::rgba8{tint, tr::norm_cast<u8>(0.4f)});
		usize trail_index{1};
		std::vector<u16>::iterator indices_it{trail.indices.begin()};
		for (usize i = 0; i < m_trail.size(); ++i) {
			// Cull unnecessary trail vertices.
			if (i < m_trail.size() - 1) {
				const glm::vec2 prev{i == 0 ? m_hitbox.c : m_trail[i - 1]};
				if (tr::collinear(prev, m_trail[i], m_trail[i + 1])) {
					continue;
				}
			}

			const u8 opacity{tr::norm_cast<u8>((m_trail.size() - i - 1) * 0.4f / m_trail.size())};
			const auto positions{trail.positions | std::views::drop(trail_index * vertices) | std::views::take(vertices)};
			const auto colors{trail.colors | std::views::drop(trail_index * vertices) | std::views::take(vertices)};
			tr::fill_circle_vertices(positions, {m_trail[i], m_hitbox.r});
			std::ranges::fill(colors, tr::rgba8{tint, opacity});
			for (usize j = 0; j < vertices; ++j) {
				*indices_it++ = u16(trail.base_index + trail_index * vertices + j);
				*indices_it++ = u16(trail.base_index + trail_index * vertices + (j + 1) % vertices);
				*indices_it++ = u16(trail.base_index + (trail_index - 1) * vertices + (j + 1) % vertices);
				*indices_it++ = u16(trail.base_index + trail_index * vertices + j);
				*indices_it++ = u16(trail.base_index + (trail_index - 1) * vertices + (j + 1) % vertices);
				*indices_it++ = u16(trail.base_index + (trail_index - 1) * vertices + j);
			}
			++trail_index;
		}
	}
}