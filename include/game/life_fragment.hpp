///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides a life fragment game object class.                                                                                           //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../global.hpp"
#include "../timer.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The size of a life fragment.
constexpr float LIFE_FRAGMENT_LENGTH{20};

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
	// Constructs an inactive life fragment.
	life_fragment() = default;

	// Gets whether the fragment is collectible.
	bool collectible() const;
	// Gets whether the fragment has been collected.
	bool collected() const;
	// Gets the hitbox of the fragment.
	const tr::circle hitbox() const;

	// Sets the fragment as collected.
	void set_collected();
	// Respawns the fragment.
	void respawn(tr::xorshiftr_128p& rng, const tr::frect2& region);
	// Updates the fragment.
	void tick();

	// Adds the fragment to the renderer.
	void add_to_renderer() const;

  private:
	// States the fragment can be in.
	enum class state {
		INACTIVE,          // The fragment is inactive.
		COLLECTIBLE,       // The fragment is collectible.
		COLLECTED,         // The fragment is in the collection animation.
		INACTIVE_COLLECTED // The fragment is inactive after being collected.
	};

	// The position of the fragment.
	glm::vec2 m_pos{};
	// The rotation of the fragment.
	tr::angle m_rot{};
	// The angular velocity of the fragment.
	tr::angle m_rotvel{};
	// The current state of the fragment.
	state m_state{state::INACTIVE};
	// Time elapsed since the current state began.
	ticks m_elapsed{0};

	// Adds the pulsing effect to the renderer.
	void add_pulse_to_renderer(tr::rgb8 color) const;
	// Adds the initial spawn wave to the renderer.
	void add_spawn_wave_to_renderer(tr::rgb8 color) const;
};