#include "../include/audio.hpp"
#include "../include/settings.hpp"

namespace engine {
	// The filenames of the sound effect audio files.
	std::array<const char*, static_cast<int>(sound::COUNT)> SFX_FILENAMES{
		"hover.ogg",   "hold.ogg", "confirm.ogg", "cancel.ogg", "type.ogg",      "pause.ogg",
		"unpause.ogg", "tick.ogg", "bounce.ogg",  "hit.ogg",    "game_over.ogg",
	};

	// Sound effect audio buffers.
	std::array<std::optional<tr::audio::buffer>, static_cast<int>(sound::COUNT)> sounds;
	// The song audio source.
	std::optional<tr::audio::source> current_song;

	// Tries to find a path to an audio file for a song.
	std::filesystem::path find_song_path(std::string_view name);
	// Tries to load an audio file.
	std::optional<tr::audio::buffer> load_audio_file(const char* filename);
} // namespace engine

std::filesystem::path engine::find_song_path(std::string_view name)
{
	std::filesystem::path path{cli_settings.datadir / "music" / std::format("{}.ogg", name)};
	if (!std::filesystem::exists(path)) {
		path = cli_settings.userdir / "music" / std::format("{}.ogg", name);
		if (!std::filesystem::exists(path)) {
			return {};
		}
	}
	return path;
}

std::optional<tr::audio::buffer> engine::load_audio_file(const char* filename)
{
	try {
		const std::filesystem::path& path{cli_settings.datadir / "sounds" / filename};
		std::optional<tr::audio::buffer> buffer{tr::audio::load_file(path)};
		LOG(tr::severity::INFO, "Loaded audio file '{}'.", filename);
		LOG_CONTINUE("From: '{}'", path.string());
		return buffer;
	}
	catch (tr::audio::file_open_error& err) {
		LOG(tr::severity::ERROR, "Failed to load audio file '{}'.", filename);
		LOG_CONTINUE("{}", err.description());
		return std::nullopt;
	}
}

void engine::initialize_audio()
{
	try {
		tr::audio::initialize();
		LOG(tr::severity::INFO, "Initialized the audio system.");
		tr::audio::set_master_gain(2);
		tr::audio::set_class_gain(0, settings.sfx_volume / 100.0f);
		tr::audio::set_class_gain(1, settings.music_volume / 100.0f);
		for (int i = 0; i < static_cast<int>(sound::COUNT); ++i) {
			sounds[i] = load_audio_file(SFX_FILENAMES[i]);
		}
		current_song.emplace(1000);
		current_song->set_classes(2);
	}
	catch (tr::audio::init_error& err) {
		LOG(tr::severity::ERROR, "Failed to initialize the audio system.");
		LOG_CONTINUE("{}", err.description());
	}
}

void engine::play_sound(sound sound, float volume, float pan, float pitch)
{
	if (tr::audio::active() && sounds[static_cast<int>(sound)].has_value()) {
		std::optional<tr::audio::source> source{tr::audio::try_allocating_source(0)};
		if (source.has_value()) {
			source->use(*sounds[static_cast<int>(sound)]);
			source->set_classes(1);
			source->set_gain(volume);
			source->set_pitch(pitch);
			source->set_pos({tr::magth(1.0f, tr::acos(pan)), 0});
			source->set_rolloff(1.0f);
			source->play();
		}
	}
}

void engine::play_song(std::string_view name, tr::fsecs fade_in)
{
	play_song(name, 0s, fade_in);
}

void engine::play_song(std::string_view name, tr::fsecs offset, tr::fsecs fade_in)
{
	if (current_song.has_value()) {
		const std::filesystem::path& path{find_song_path(name)};
		if (path.empty()) {
			LOG(tr::severity::ERROR, "Failed to play song '{}'.", name);
			LOG_CONTINUE("File not found in neither data nor user directory.");
			return;
		}
		try {
			current_song->stop();
			current_song->use(tr::audio::open_file(path));
			current_song->set_offset(offset);
			current_song->set_gain(0.25f);
			current_song->set_gain(0.75f, fade_in);
			current_song->play();
			LOG(tr::severity::INFO, "Playing song '{}'.", name);
			LOG_CONTINUE("From: '{}'", path.string());
		}
		catch (tr::exception& err) {
			LOG(tr::severity::ERROR, "Failed to play song '{}'.", name);
			LOG_CONTINUE("From: '{}'", path.string());
			LOG_CONTINUE(err);
		}
	}
}

void engine::pause_song()
{
	if (current_song.has_value()) {
		current_song->pause();
	}
}

void engine::unpause_song()
{
	if (current_song.has_value()) {
		current_song->play();
		current_song->set_gain(0);
		current_song->set_gain(0.75f, 0.5s);
	}
}

void engine::fade_song_out(tr::fsecs time)
{
	if (current_song.has_value()) {
		current_song->set_gain(0, time);
	}
}

void engine::apply_audio_settings()
{
	if (tr::audio::active()) {
		tr::audio::set_class_gain(0, settings.sfx_volume / 100.0f);
		tr::audio::set_class_gain(1, settings.music_volume / 100.0f);
	}
}

void engine::shut_down_audio()
{
	if (tr::audio::active()) {
		current_song.reset();
		for (std::optional<tr::audio::buffer>& sound : sounds) {
			sound.reset();
		}
		tr::audio::shut_down();
		LOG(tr::severity::INFO, "Shut down the audio system.");
	}
}