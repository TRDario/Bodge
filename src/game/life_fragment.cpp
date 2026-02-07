#include "../../include/game/life_fragment.hpp"
#include "../../include/renderer.hpp"
#include "../../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Minimum coordinate that a life fragment can spawn at.
constexpr float LIFE_FRAGMENT_MIN_POS{FIELD_MIN + LIFE_FRAGMENT_LENGTH};
// Maximum coordinate that a life fragment can spawn at.
constexpr float LIFE_FRAGMENT_MAX_POS{FIELD_MAX - LIFE_FRAGMENT_LENGTH};

// Duration of the fragment spawn animation.
constexpr ticks LIFE_FRAGMENT_SPAWN_ANIMATION_TIME{1.5_s};
// Duration of the fragment collection animation.
constexpr ticks LIFE_FRAGMENT_COLLECTION_ANIMATION_TIME{0.5_s};

////////////////////////////////////////////////////////////// LIFE FRAGMENT //////////////////////////////////////////////////////////////

bool life_fragment::collectible() const
{
	return m_state == state::COLLECTIBLE;
}

bool life_fragment::collected() const
{
	return m_state == state::COLLECTED || m_state == state::INACTIVE_COLLECTED;
}

const tr::circle life_fragment::hitbox() const
{
	return {m_pos, LIFE_FRAGMENT_LENGTH};
}

void life_fragment::set_collected()
{
	m_state = state::COLLECTED;
	m_elapsed = 0;
}

void life_fragment::respawn(tr::xorshiftr_128p& rng, const tr::frect2& region)
{
	m_pos = {rng.generate(std::max(region.tl.x, LIFE_FRAGMENT_MIN_POS), std::min(region.tl.x + region.size.x, LIFE_FRAGMENT_MAX_POS)),
			 rng.generate(std::max(region.tl.y, LIFE_FRAGMENT_MIN_POS), std::min(region.tl.y + region.size.y, LIFE_FRAGMENT_MAX_POS))};
	m_rot = rng.generate_angle();
	m_rotvel = rng.generate(0.4_deg, 0.75_deg) * rng.generate_sign();
	m_state = state::COLLECTIBLE;
	m_elapsed = 0;
}

void life_fragment::tick()
{
	m_elapsed++;
	switch (m_state) {
	case state::INACTIVE:
	case state::INACTIVE_COLLECTED:
		break;
	case state::COLLECTIBLE:
		m_rot += m_rotvel * std::clamp(1.0f / (m_elapsed / float(LIFE_FRAGMENT_SPAWN_ANIMATION_TIME)), 1.0f, 5.0f);
		if (m_elapsed >= LIFE_FRAGMENT_DURATION) {
			m_state = state::INACTIVE;
			m_elapsed = 0;
		}
		break;
	case state::COLLECTED:
		m_rot += m_rotvel * std::clamp(1.0f / m_elapsed * LIFE_FRAGMENT_SPAWN_ANIMATION_TIME, 1.0f, 2.5f);
		if (m_elapsed >= LIFE_FRAGMENT_COLLECTION_ANIMATION_TIME) {
			m_state = state::INACTIVE_COLLECTED;
			m_elapsed = 0;
		}
		break;
	}
}

void life_fragment::add_to_renderer() const
{
	if (m_state == state::INACTIVE || m_state == state::INACTIVE_COLLECTED) {
		return;
	}

	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{float(g_settings.primary_hue), 1, 1})};

	float raw_factor;
	float eased_factor;
	glm::vec2 size;
	u8 opacity;
	if (m_state == state::COLLECTED) {
		raw_factor = std::min(m_elapsed / float(LIFE_FRAGMENT_COLLECTION_ANIMATION_TIME), 1.0f);
		eased_factor = raw_factor == 1.0f ? raw_factor : 1.0f - std::pow(2.0f, -10.0f * raw_factor);
		size = {4 * (1 + 4 * eased_factor), LIFE_FRAGMENT_LENGTH * (1 + 4 * eased_factor)};
		opacity = tr::norm_cast<u8>(0.75f - 0.75f * raw_factor);
	}
	else {
		raw_factor = std::min(m_elapsed / float(LIFE_FRAGMENT_SPAWN_ANIMATION_TIME), 1.0f);
		eased_factor = raw_factor == 1.0f ? raw_factor : 1.0f - std::pow(2.0f, -10.0f * raw_factor);
		size = {4 * (11 - 10 * eased_factor), LIFE_FRAGMENT_LENGTH * (11 - 10 * eased_factor)};
		opacity = tr::norm_cast<u8>(std::pow(raw_factor, 1 / 1.5f));
		if (m_elapsed >= LIFE_FRAGMENT_FAST_FLASH_START) {
			opacity = opacity * (tr::turns(m_elapsed / 0.2_sf).cos() + 1) / 2;
		}
		else if (m_elapsed >= LIFE_FRAGMENT_SLOW_FLASH_START) {
			opacity = opacity * (tr::turns(m_elapsed / 0.5_sf).cos() + 1) / 2;
		}

		if (m_elapsed >= LIFE_FRAGMENT_SPAWN_ANIMATION_TIME) {
			add_pulse_to_renderer(color);
		}
		else {
			add_spawn_wave_to_renderer(color);
		}
	}

	const tr::gfx::simple_color_mesh_ref mesh{g_renderer->basic.new_color_fan(layer::LIFE_FRAGMENTS, 4)};
	tr::fill_rectangle_vertices(mesh.positions, m_pos, size / 2.0f, size, m_rot);
	std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
}

void life_fragment::add_pulse_to_renderer(tr::rgb8 color) const
{
	const float t{(m_elapsed - LIFE_FRAGMENT_SPAWN_ANIMATION_TIME + 1) % 1_s / float(1_s)};
	const float scale{std::pow(t, 1 / 2.5f) * 75};
	const u8 opacity{tr::norm_cast<u8>(std::max(0.75f - std::sqrt(0.75f * t), 0.0f))};

	if (opacity > 0 && scale > 0) {
		g_renderer->circle.add_circle_outline(layer::LIFE_FRAGMENTS, {m_pos, scale}, 2, tr::rgba8{color, opacity});
	}
}

void life_fragment::add_spawn_wave_to_renderer(tr::rgb8 color) const
{
	const float t{m_elapsed / float(LIFE_FRAGMENT_SPAWN_ANIMATION_TIME)};
	const float scale{std::pow(t, 2.0f) * 200};
	const u8 opacity{tr::norm_cast<u8>(std::sqrt(1 - t))};

	if (opacity > 0 && scale > 0) {
		g_renderer->circle.add_circle_outline(layer::LIFE_FRAGMENTS, {m_pos, scale}, 2, tr::rgba8{color, opacity});
	}
}