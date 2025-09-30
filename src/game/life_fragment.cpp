#include "../../include/game/life_fragment.hpp"
#include "../../include/graphics.hpp"
#include "../../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The size of a life fragment.
constexpr float LIFE_FRAGMENT_LENGTH{20};
constexpr float LIFE_FRAGMENT_MIN_POS{FIELD_MIN + LIFE_FRAGMENT_LENGTH};
constexpr float LIFE_FRAGMENT_MAX_POS{FIELD_MAX - LIFE_FRAGMENT_LENGTH};

// The duration of the fragment spawn animation.
constexpr ticks LIFE_FRAGMENT_SPAWN_ANIMATION_TIME{1.5_s};
// The duration of the fragment collection animation.
constexpr ticks LIFE_FRAGMENT_COLLECTION_ANIMATION_TIME{0.5_s};

////////////////////////////////////////////////////////////// LIFE FRAGMENT //////////////////////////////////////////////////////////////

life_fragment::life_fragment(tr::xorshiftr_128p& rng, const tr::frect2& region)
	: m_pos{rng.generate(std::max(region.tl.x, LIFE_FRAGMENT_MIN_POS), std::min(region.tl.x + region.size.x, LIFE_FRAGMENT_MAX_POS)),
			rng.generate(std::max(region.tl.y, LIFE_FRAGMENT_MIN_POS), std::min(region.tl.y + region.size.y, LIFE_FRAGMENT_MAX_POS))}
	, m_rot{rng.generate_angle()}
	, m_rotvel{rng.generate(0.4_deg, 0.75_deg) * rng.generate_sign()}
{
}

bool life_fragment::collected() const
{
	return m_collected_timer.active();
}

bool life_fragment::can_despawn() const
{
	return collected() && m_collected_timer.elapsed() >= LIFE_FRAGMENT_COLLECTION_ANIMATION_TIME;
}

const tr::circle life_fragment::hitbox() const
{
	return {m_pos, LIFE_FRAGMENT_LENGTH};
}

void life_fragment::set_collected()
{
	m_collected_timer.start();
}

void life_fragment::update(ticks time_since_spawned)
{
	if (m_collected_timer.active()) {
		m_collected_timer.update();
		m_rot += m_rotvel * std::clamp(1.0f / m_collected_timer.elapsed() * LIFE_FRAGMENT_SPAWN_ANIMATION_TIME, 1.0f, 2.5f);
	}
	else {
		m_rot += m_rotvel * std::clamp(1.0f / ((time_since_spawned + 1.0f) / LIFE_FRAGMENT_SPAWN_ANIMATION_TIME), 1.0f, 5.0f);
	}
}

void life_fragment::add_to_renderer(ticks time_since_spawned) const
{
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{float(engine::settings.primary_hue), 1, 1})};

	float raw_age_factor;
	float eased_age_factor;
	glm::vec2 size;
	u8 opacity;
	if (collected()) {
		raw_age_factor = std::min(m_collected_timer.elapsed() / float(LIFE_FRAGMENT_COLLECTION_ANIMATION_TIME), 1.0f);
		eased_age_factor = raw_age_factor == 1.0f ? raw_age_factor : 1.0f - std::pow(2.0f, -10.0f * raw_age_factor);
		size = {4 * (1 + 4 * eased_age_factor), LIFE_FRAGMENT_LENGTH * (1 + 4 * eased_age_factor)};
		opacity = tr::norm_cast<u8>(0.75f - 0.75f * raw_age_factor);
	}
	else {
		raw_age_factor = std::min(time_since_spawned / float(LIFE_FRAGMENT_SPAWN_ANIMATION_TIME), 1.0f);
		eased_age_factor = raw_age_factor == 1.0f ? raw_age_factor : 1.0f - std::pow(2.0f, -10.0f * raw_age_factor);
		size = {4 * (11 - 10 * eased_age_factor), LIFE_FRAGMENT_LENGTH * (11 - 10 * eased_age_factor)};
		opacity = tr::norm_cast<u8>(std::pow(raw_age_factor, 1 / 1.5f));
		if (time_since_spawned >= LIFE_FRAGMENT_FAST_FLASH_START) {
			opacity = opacity * (tr::turns(time_since_spawned / 0.2_sf).cos() + 1) / 2;
		}
		else if (time_since_spawned >= LIFE_FRAGMENT_SLOW_FLASH_START) {
			opacity = opacity * (tr::turns(time_since_spawned / 0.5_sf).cos() + 1) / 2;
		}

		if (time_since_spawned >= LIFE_FRAGMENT_SPAWN_ANIMATION_TIME) {
			add_pulse_to_renderer(color, time_since_spawned);
		}
		else {
			add_spawn_wave_to_renderer(color, time_since_spawned);
		}
	}

	const tr::gfx::simple_color_mesh_ref mesh{engine::basic_renderer().new_color_fan(layer::LIFE_FRAGMENTS, 4)};
	tr::fill_rectangle_vertices(mesh.positions, m_pos, size / 2.0f, size, m_rot);
	std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
}

void life_fragment::add_pulse_to_renderer(tr::rgb8 color, ticks time_since_spawned) const
{
	const float t{(time_since_spawned - LIFE_FRAGMENT_SPAWN_ANIMATION_TIME + 1) % 1_s / float(1_s)};
	const float scale{std::pow(t, 1 / 2.5f) * 75};
	const u8 opacity{tr::norm_cast<u8>(std::max(0.75f - std::sqrt(0.75f * t), 0.0f))};

	if (opacity > 0 && scale > 0) {
		const usize vertices{tr::smooth_polygon_vertices(scale * engine::render_scale())};

		const tr::gfx::simple_color_mesh_ref mesh{engine::basic_renderer().new_color_outline(layer::LIFE_FRAGMENTS, vertices)};
		tr::fill_polygon_outline_vertices(mesh.positions, {m_pos, scale}, 0_deg, 2);
		std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
	}
}

void life_fragment::add_spawn_wave_to_renderer(tr::rgb8 color, ticks time_since_spawned) const
{
	const float t{time_since_spawned / float(LIFE_FRAGMENT_SPAWN_ANIMATION_TIME)};
	const float scale{std::pow(t, 2.0f) * 200};
	const u8 opacity{tr::norm_cast<u8>(std::sqrt(1 - t))};

	if (opacity > 0 && scale > 0) {
		const usize vertices{tr::smooth_polygon_vertices(scale * engine::render_scale())};

		const tr::gfx::simple_color_mesh_ref mesh{engine::basic_renderer().new_color_outline(layer::LIFE_FRAGMENTS, vertices)};
		tr::fill_polygon_outline_vertices(mesh.positions, {m_pos, scale}, 0_deg, 2);
		std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
	}
}