#pragma once
#include "global.hpp"

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
constexpr tr::fsecs SKIP_MENU_SONG_INTRO_TIMESTAMP{103769 / 44100.0f};

std::vector<std::string> create_available_song_list();

inline class audio {
  public:
	void initialize();
	void apply_settings();
	void shut_down();

	void play_sound(sound sound, float volume, float pan, float pitch = 1);
	void play_song(std::string_view name, tr::fsecs fade_in);
	void play_song(std::string_view name, tr::fsecs offset, tr::fsecs fade_in);
	void pause_song();
	void unpause_song();
	void fade_song_out(tr::fsecs time);

  private:
	std::array<std::optional<tr::audio::buffer>, int(sound::COUNT)> m_sounds;
	std::optional<tr::audio::source> m_current_song;
} g_audio;