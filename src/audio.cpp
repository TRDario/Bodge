#include "../include/audio.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////// IMPLEMENTATION /////////////////////////////////////////////////////////////

// Actual audio file IDs.
enum audio_file {
	// Sentinel for the first audio file.
	FIRST,

	CLICK = FIRST,
	CONFIRM,
	BACK,
	HIT,
	EXPLOSION,
	BOUNCE1,

	// The total number of sounds.
	COUNT
};

// The filenames of the audio files.
array<const char*, audio_file::COUNT> AUDIO_FILENAMES{
	"click.ogg", "confirm.ogg", "back.ogg", "hit.ogg", "explosion.ogg", "bounce1.ogg",
};

// Converts a sound effect name to an actual audio file.
audio_file sfx_to_audio_file(sfx sfx) noexcept
{
	switch (sfx) {
	case sfx::CLICK:
		return audio_file::CLICK;
	case sfx::CONFIRM:
		return audio_file::CLICK;
	case sfx::BACK:
		return audio_file::CLICK;
	case sfx::HIT:
		return audio_file::CLICK;
	case sfx::EXPLOSION:
		return audio_file::CLICK;
	case sfx::BOUNCE:
		return audio_file::BOUNCE1;
	}
}

// Sound effect audio buffers.
array<optional<audio_buffer>, audio_file::COUNT> sounds;

// Tries to load an audio file.
optional<audio_buffer> load_audio_file(const char* filename) noexcept
{
	try {
		optional<audio_buffer> buffer{tr::load_audio_file(cli_settings.datadir / "audio" / filename)};
		LOG(INFO, "Loaded audio from '{}'.", (cli_settings.datadir / "audio" / filename).string());
		return buffer;
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to load audio from '{}': {}.", (cli_settings.datadir / "audio" / filename).string(), err.what());
		return std::nullopt;
	}
}

////////////////////////////////////////////////////////////////// AUDIO //////////////////////////////////////////////////////////////////

void audio::initialize() noexcept
{
	try {
		audio_system::initialize();
		LOG(INFO, "Initialized the audio system.");
		audio_system::set_master_gain(2);
		audio_system::set_class_gain(0, settings.sfx_volume / 100.0f);
		audio_system::set_class_gain(1, settings.music_volume / 100.0f);
		for (audio_file i = audio_file::FIRST; i < audio_file::COUNT; i = static_cast<audio_file>(static_cast<int>(i) + 1)) {
			sounds[i] = load_audio_file(AUDIO_FILENAMES[i]);
		}
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to initialize the audio system: {}.", err.what());
	}
}

void audio::play(sfx sfx, float volume, float pan, float pitch) noexcept
{
	const audio_file file{sfx_to_audio_file(sfx)};
	if (audio_system::active() && sounds[file].has_value()) {
		try {
			audio_source source{0};
			source.use(*sounds[file]);
			source.set_classes(1);
			source.set_gain(volume);
			source.set_pitch(pitch);
			source.set_pos({magth(1.0f, tr::acos(pan)), 0});
			source.set_rolloff(1.0f);
			source.play();
		}
		catch (...) {
		}
	}
}

void audio::apply_settings() noexcept
{
	if (audio_system::active()) {
		audio_system::set_class_gain(0, settings.sfx_volume / 100.0f);
		audio_system::set_class_gain(1, settings.music_volume / 100.0f);
	}
}

void audio::shut_down() noexcept
{
	if (audio_system::active()) {
		for (optional<audio_buffer>& sound : sounds) {
			sound.reset();
		}
		audio_system::shut_down();
		LOG(INFO, "Shut down the audio system.");
	}
}