#pragma once

// Sound effects.
enum class sound {
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

// Timestamp that skips the menu song intro.
constexpr tr::fsecs SKIP_MENU_SONG_INTRO{103769 / 44100.0f};

// Audio system.
namespace engine {
	// Initializes the audio system.
	// Note that the program has a high tolerance to audio errors and will even survive a complete failure.
	void initialize_audio();
	// Creates a list containing the names of available songs.
	std::vector<std::string> create_available_song_list();
	// Plays a sound effect.
	void play_sound(sound sound, float volume, float pan, float pitch = 1);
	// Plays a song.
	void play_song(std::string_view name, tr::fsecs fade_in);
	// Plays a song starting at an offset.
	void play_song(std::string_view name, tr::fsecs offset, tr::fsecs fade_in);
	// Pauses the current song.
	void pause_song();
	// Unpauses the current song.
	void unpause_song();
	// Fades the current song out.
	void fade_song_out(tr::fsecs time);
	// Applies new settings.
	void apply_audio_settings();
	// Shuts the audio system down.
	void shut_down_audio();
}; // namespace engine