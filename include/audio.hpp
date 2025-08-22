#pragma once

enum class sound {
	HOVER,
	HOLD,
	CONFIRM,
	CANCEL,
	TYPE,
	PAUSE,
	UNPAUSE,
	TICK,
	BOUNCE,
	HIT,
	GAME_OVER,

	// The total number of sound effects.
	COUNT
};

constexpr tr::fsecs SKIP_MENU_SONG_INTRO_TIMESTAMP{103769 / 44100.0f};

namespace engine {
	void initialize_audio();
	std::vector<std::string> create_available_song_list();
	void play_sound(sound sound, float volume, float pan, float pitch = 1);
	void play_song(std::string_view name, tr::fsecs fade_in);
	void play_song(std::string_view name, tr::fsecs offset, tr::fsecs fade_in);
	void pause_song();
	void unpause_song();
	void fade_song_out(tr::fsecs time);
	void apply_audio_settings();
	void shut_down_audio();
}; // namespace engine