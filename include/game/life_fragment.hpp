#pragma once
#include "../global.hpp"
#include "../time.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Total duration of a life fragment event.
constexpr ticks LIFE_FRAGMENT_DURATION{10_s};
// The starting time during an event of fragment slow flashing.
constexpr ticks LIFE_FRAGMENT_SLOW_FLASH_START{LIFE_FRAGMENT_DURATION - 3.0_s};
// The starting time during an event of fragment fast flashing.
constexpr ticks LIFE_FRAGMENT_FAST_FLASH_START{LIFE_FRAGMENT_DURATION - 1.0_s};

////////////////////////////////////////////////////////////// LIFE FRAGMENT //////////////////////////////////////////////////////////////

class life_fragment {
  public:
	// Randomly generates a fragment in a region.
	life_fragment(tr::xorshiftr_128p& rng, const tr::frect2& region);

	bool collected() const;
	bool can_despawn() const;
	const tr::circle hitbox() const;

	void set_collected();
	void update(ticks time_since_spawned);

	void add_to_renderer(ticks time_since_spawned) const;

  private:
	glm::vec2 m_pos;
	tr::angle m_rot;
	tr::angle m_rotvel;
	startable_timer m_collected_timer;

	void add_pulse_to_renderer(tr::rgb8 color, ticks time_since_spawned) const;
	void add_spawn_wave_to_renderer(tr::rgb8 color, ticks time_since_spawned) const;
};