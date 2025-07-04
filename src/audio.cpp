#include "../include/audio.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////// IMPLEMENTATION /////////////////////////////////////////////////////////////

// Actual audio file IDs.
enum audio_file {
	// Sentinel for the first audio file.
	FIRST,

	HOVER = FIRST,
	HOLD,
	CONFIRM,
	CANCEL,
	TYPE,

	// The total number of sounds.
	COUNT
};

// The filenames of the audio files.
std::array<const char*, audio_file::COUNT> AUDIO_FILENAMES{"hover.ogg", "hold.ogg", "confirm.ogg", "cancel.ogg", "type.ogg"};

// Converts a sound effect name to an actual audio file.
audio_file sfx_to_audio_file(sfx sfx) noexcept
{
	switch (sfx) {
	case sfx::HOVER:
		return audio_file::HOVER;
	case sfx::HOLD:
		return audio_file::HOLD;
	case sfx::CONFIRM:
		return audio_file::CONFIRM;
	case sfx::CANCEL:
		return audio_file::CANCEL;
	case sfx::TYPE:
		return audio_file::TYPE;
	}
}

// Sound effect audio buffers.
std::array<std::optional<tr::audio_buffer>, audio_file::COUNT> sounds;

// Tries to load an audio file.
std::optional<tr::audio_buffer> load_audio_file(const char* filename) noexcept
{
	try {
		std::optional<tr::audio_buffer> buffer{tr::load_audio_file(cli_settings.datadir / "sfx" / filename)};
		LOG(tr::severity::INFO, "Loaded audio from '{}'.", (cli_settings.datadir / "sfx" / filename).string());
		return buffer;
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load audio from '{}': {}.", (cli_settings.datadir / "sfx" / filename).string(), err.what());
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
		for (audio_file i = audio_file::FIRST; i < audio_file::COUNT; i = static_cast<audio_file>(static_cast<int>(i) + 1)) {
			sounds[i] = load_audio_file(AUDIO_FILENAMES[i]);
		}
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to initialize the audio system: {}.", err.what());
	}
}

void audio::play(sfx sfx, float volume, float pan, float pitch) noexcept
{
	const audio_file file{sfx_to_audio_file(sfx)};
	if (tr::audio_system::active() && sounds[file].has_value()) {
		try {
			tr::audio_source source{0};
			source.use(*sounds[file]);
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