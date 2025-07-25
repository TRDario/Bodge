#pragma once

// Sound effects.
enum class sfx {
	// UI button hover.
	HOVER,
	// UI button hold.
	HOLD,
	// Affirmative action.
	CONFIRM,
	// Negative action.
	CANCEL,
	// Typing.
	TYPE,
	// Pausing the game.
	PAUSE,
	// Unpausing the game.
	UNPAUSE,
	// Clock tick.
	TICK,
	// Ball bounce.
	BOUNCE,
	// Player hit.
	HIT,
	// Game over.
	GAME_OVER,

	// The total number of sound effects.
	COUNT,
};

// Songs.
enum class song {
	// Menu theme.
	MENU,

	// The total number of songs.
	COUNT
};

// Audio system.
struct audio {
	// Initializes the audio system.
	// Note that the program has a high tolerance to audio errors and will even survive a complete failure.
	static void initialize() noexcept;
	// Plays a sound effect.
	static void play(sfx sfx, float volume, float pan, float pitch = 1) noexcept;
	// Plays a song.
	static void play(song song) noexcept;
	// Pauses the current song.
	static void pause_song() noexcept;
	// Fades the current song out.
	static void fade_song_out(tr::fsecs time) noexcept;
	// Applies new settings.
	static void apply_settings() noexcept;
	// Shuts the audio system down.
	static void shut_down() noexcept;
};