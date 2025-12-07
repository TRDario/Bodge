#pragma once
#include "global.hpp"

// List of available sound effects.
enum class sound {
	HOVER,
	HOLD,
	CONFIRM,
	CANCEL,
	TYPE,
	PAUSE,
	UNPAUSE,
	TICK,
	TICK_ALT,
	BALL_SPAWN,
	BOUNCE,
	STYLE,
	FRAGMENT_SPAWN,
	COLLECT,
	ONE_UP,
	HIT,
	GAME_OVER,

	// The total number of sound effects.
	COUNT
};

// Timestamp to skip the initial portion of the menu song when returning from other states.
inline constexpr tr::fsecs SKIP_MENU_SONG_INTRO_TIMESTAMP{103769 / 44100.0f};

// Creates a list of filenames of available songs.
std::vector<std::string> create_available_song_list();

// Audio manager class.
inline class audio {
  public:
	// Initializes the audio manager.
	void initialize();
	// Applies audio settings.
	void apply_settings();
	// Shuts down the audio manager.
	void shut_down();

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

  private:
	// Loaded sound effect data.
	std::array<std::optional<tr::audio::buffer>, int(sound::COUNT)> m_sounds;
	// The currently playing song.
	std::optional<tr::audio::source> m_current_song;
} g_audio; // Global audio manager.