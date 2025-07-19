#include "../include/audio.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////// IMPLEMENTATION /////////////////////////////////////////////////////////////

// The filenames of the audio files.
std::array<const char*, static_cast<int>(sfx::COUNT)> AUDIO_FILENAMES{
	"hover.ogg",   "hold.ogg", "confirm.ogg", "cancel.ogg", "type.ogg",      "pause.ogg",
	"unpause.ogg", "tick.ogg", "bounce.ogg",  "hit.ogg",    "game_over.ogg",
};

// Sound effect audio buffers.
std::array<std::optional<tr::audio_buffer>, static_cast<int>(sfx::COUNT)> sounds;

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

void audio::initialize() noexcept
{
	try {
		tr::audio_system::initialize();
		LOG(tr::severity::INFO, "Initialized the audio system.");
		tr::audio_system::set_master_gain(2);
		tr::audio_system::set_class_gain(0, settings.sfx_volume / 100.0f);
		tr::audio_system::set_class_gain(1, settings.music_volume / 100.0f);
		for (int i = 0; i < static_cast<int>(sfx::COUNT); ++i) {
			sounds[i] = load_audio_file(AUDIO_FILENAMES[i]);
		}
	}
	catch (tr::audio_system_init_error& err) {
		LOG(tr::severity::ERROR, "Failed to initialize the audio system.");
		LOG_CONTINUE("{}", err.description());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to initialize the audio system.");
		LOG_CONTINUE("{}", err.what());
	}
}

void audio::play(sfx sfx, float volume, float pan, float pitch) noexcept
{
	if (tr::audio_system::active() && sounds[static_cast<int>(sfx)].has_value()) {
		try {
			tr::audio_source source{0};
			source.use(*sounds[static_cast<int>(sfx)]);
			source.set_classes(1);
			source.set_gain(volume);
			source.set_pitch(pitch);
			source.set_pos({tr::magth(1.0f, tr::acos(pan)), 0});
			source.set_rolloff(1.0f);
			source.play();
		}
		catch (...) {
		}
	}
}

void audio::apply_settings() noexcept
{
	if (tr::audio_system::active()) {
		tr::audio_system::set_class_gain(0, settings.sfx_volume / 100.0f);
		tr::audio_system::set_class_gain(1, settings.music_volume / 100.0f);
	}
}

void audio::shut_down() noexcept
{
	if (tr::audio_system::active()) {
		for (std::optional<tr::audio_buffer>& sound : sounds) {
			sound.reset();
		}
		tr::audio_system::shut_down();
		LOG(tr::severity::INFO, "Shut down the audio system.");
	}
}