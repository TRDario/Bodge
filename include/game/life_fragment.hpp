#pragma once
#include "../global.hpp"
#include "../timer.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Total duration of a life fragment event.
constexpr ticks LIFE_FRAGMENT_DURATION{10_s};
// The starting time during an event of fragment slow flashing.
constexpr ticks LIFE_FRAGMENT_SLOW_FLASH_START{LIFE_FRAGMENT_DURATION - 3.0_s};
// The starting time during an event of fragment fast flashing.
constexpr ticks LIFE_FRAGMENT_FAST_FLASH_START{LIFE_FRAGMENT_DURATION - 1.0_s};

////////////////////////////////////////////////////////////// LIFE FRAGMENT //////////////////////////////////////////////////////////////

// Collectible life fragment.
class life_fragment {
  public:
	// Randomly generates a fragment in a region.
	life_fragment(tr::xorshiftr_128p& rng, const tr::frect2& region);

	// Gets whether the fragment has been collected.
	bool collected() const;
	// Gets whether the fragment can be despawned.
	bool can_despawn() const;
	// Gets the hitbox of the fragment.
	const tr::circle hitbox() const;

	// Sets the fragment as collected.
	void set_collected();
	// Updates the fragment.
	void tick();

	// Adds the fragment to the renderer.
	void add_to_renderer() const;

  private:
	// The position of the fragment.
	glm::vec2 m_pos;
	// The rotation of the fragment.
	tr::angle m_rot;
	// The angular velocity of the fragment.
	tr::angle m_rotvel;
	// The age of the fragment.
	ticks m_age;
	// Timer that is started after the fragment is collected.
	startable_timer m_collected_timer;

	// Adds the pulsing effect to the renderer.
	void add_pulse_to_renderer(tr::rgb8 color) const;
	// Adds the initial spawn wave to the renderer.
	void add_spawn_wave_to_renderer(tr::rgb8 color) const;
};