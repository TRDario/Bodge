#pragma once

// Sound effects.
enum class sfx {
	// Menu button click sound effect.
	CLICK,
	// Menu button selection and click release sound effect.
	CONFIRM,
	// Sound effect for menu shrotcuts that go back.
	BACK,
	// Player hit sound effect.
	HIT,
	// Game over explosion sound effect.
	EXPLOSION,
	// Ball bounce sound effects.
	BOUNCE
};

// Audio system.
struct audio {
	// Initializes the audio system.
	// Note that the program has a high tolerance to audio errors and will even survive a complete failure.
	static void initialize() noexcept;
	// Plays a sound effect.
	static void play(sfx sfx, float volume, float pan, float pitch = 1) noexcept;
	// Applies new settings.
	static void apply_settings() noexcept;
	// Shuts the audio system down.
	static void shut_down() noexcept;
};