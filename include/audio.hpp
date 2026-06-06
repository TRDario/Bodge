///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides an interface for playing sound effects and songs.                                                                            //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

// Audio manager singleton.
class audio {
  public:
	// Gets the audio manager instance.
	static audio& instance();

	// Sets the audio volume.
	void set_volume(float sfx_volume, float music_volume);

	// Plays a sound effect.
	void play_sound(sound sound, float volume, float pan, float pitch = 1);
	// Plays a song.
	void play_song(std::string_view name, tr::fsecs fade_in);
	// Plays a song starting at an offset.
	void play_song(std::string_view name, tr::fsecs offset, tr::fsecs fade_in);
	// Sets the speed of the playing song.
	void set_song_speed(float multiplier);
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

	// Initializes the audio manager.
	audio();
	// Shuts down the audio manager.
	~audio();
};