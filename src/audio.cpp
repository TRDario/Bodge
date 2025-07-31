#include "../include/audio.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////// IMPLEMENTATION /////////////////////////////////////////////////////////////

// The filenames of the sound effect audio files.
std::array<const char*, static_cast<int>(sound::COUNT)> SFX_FILENAMES{
	"hover.ogg",   "hold.ogg", "confirm.ogg", "cancel.ogg", "type.ogg",      "pause.ogg",
	"unpause.ogg", "tick.ogg", "bounce.ogg",  "hit.ogg",    "game_over.ogg",
};

// The filenames of the music audio files.
std::array<const char*, static_cast<int>(sound::COUNT)> SONG_FILENAMES{
	"menu.ogg",
};

// Sound effect audio buffers.
std::array<std::optional<tr::audio_buffer>, static_cast<int>(sound::COUNT)> sounds;
// The song audio source.
std::optional<tr::audio_source> current_song;

// Tries to load an audio file.
std::optional<tr::audio_buffer> load_audio_file(const char* filename) noexcept
{
	try {
		const std::filesystem::path& path{cli_settings.datadir / "sounds" / filename};
		std::optional<tr::audio_buffer> buffer{tr::load_audio_file(path)};
		LOG(tr::severity::INFO, "Loaded audio file '{}'.", filename);
		LOG_CONTINUE("From: '{}'", path.string());
		return buffer;
	}
	catch (tr::audio_file_open_error& err) {
		LOG(tr::severity::ERROR, "Failed to load audio file '{}'.", filename);
		LOG_CONTINUE("{}", err.description());
		return std::nullopt;
	}
}

////////////////////////////////////////////////////////////////// AUDIO //////////////////////////////////////////////////////////////////

void audio::initialize()
{
	try {
		tr::audio_system::initialize();
		LOG(tr::severity::INFO, "Initialized the audio system.");
		tr::audio_system::set_master_gain(2);
		tr::audio_system::set_class_gain(0, settings.sfx_volume / 100.0f);
		tr::audio_system::set_class_gain(1, settings.music_volume / 100.0f);
		for (int i = 0; i < static_cast<int>(sound::COUNT); ++i) {
			sounds[i] = load_audio_file(SFX_FILENAMES[i]);
		}
		current_song.emplace(1000);
		current_song->set_classes(2);
	}
	catch (tr::audio_system_init_error& err) {
		LOG(tr::severity::ERROR, "Failed to initialize the audio system.");
		LOG_CONTINUE("{}", err.description());
	}
}

void audio::play_sound(sound sound, float volume, float pan, float pitch)
{
	if (tr::audio_system::active() && sounds[static_cast<int>(sound)].has_value()) {
		try {
			tr::audio_source source{0};
			source.use(*sounds[static_cast<int>(sound)]);
			source.set_classes(1);
			source.set_gain(volume);
			source.set_pitch(pitch);
			source.set_pos({tr::magth(1.0f, tr::acos(pan)), 0});
			source.set_rolloff(1.0f);
			source.play();
		}
		catch (std::exception& err) {
			LOG(tr::severity::ERROR, "Failed to play sound effect.");
			LOG_CONTINUE("{}", err.what());
		}
	}
}

void audio::play_song(song song, tr::fsecs fade_in)
{
	play_song(song, 0s, fade_in);
}

void audio::play_song(song song, tr::fsecs offset, tr::fsecs fade_in)
{
	const char* const filename{SONG_FILENAMES[static_cast<int>(song)]};
	const std::filesystem::path& path{cli_settings.datadir / "music" / filename};
	if (current_song.has_value()) {
		try {
			current_song->stop();
			current_song->use(tr::open_audio_file(path));
			current_song->set_offset(offset);
			current_song->set_gain(0.25f);
			current_song->set_gain(0.75f, fade_in);
			current_song->play();
			LOG(tr::severity::INFO, "Playing song '{}'.", filename);
			LOG_CONTINUE("From: '{}'", path.string());
		}
		catch (std::exception& err) {
			LOG(tr::severity::ERROR, "Failed to play song '{}'.", filename);
			LOG_CONTINUE("From: '{}'", path.string());
			LOG_CONTINUE("{}", err.what());
		}
	}
}

void audio::pause_song()
{
	if (current_song.has_value()) {
		current_song->pause();
	}
}

void audio::fade_song_out(tr::fsecs time)
{
	if (current_song.has_value()) {
		current_song->set_gain(0, time);
	}
}

void audio::apply_settings()
{
	if (tr::audio_system::active()) {
		tr::audio_system::set_class_gain(0, settings.sfx_volume / 100.0f);
		tr::audio_system::set_class_gain(1, settings.music_volume / 100.0f);
	}
}

void audio::shut_down()
{
	if (tr::audio_system::active()) {
		current_song.reset();
		for (std::optional<tr::audio_buffer>& sound : sounds) {
			sound.reset();
		}
		tr::audio_system::shut_down();
		LOG(tr::severity::INFO, "Shut down the audio system.");
	}
}