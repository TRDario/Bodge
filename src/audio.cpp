#include "../include/audio.hpp"
#include "../include/settings.hpp"

namespace engine {
	std::array<const char*, int(sound::COUNT)> SFX_FILENAMES{
		"hover.ogg",          "hold.ogg",    "confirm.ogg",  "cancel.ogg",     "type.ogg",      "pause.ogg",
		"unpause.ogg",        "tick.ogg",    "tick_alt.ogg", "ball_spawn.ogg", "bounce.ogg",    "style.ogg",
		"fragment_spawn.ogg", "collect.ogg", "1up.ogg",      "hit.ogg",        "game_over.ogg",
	};

	std::array<std::optional<tr::audio::buffer>, int(sound::COUNT)> sounds;
	std::optional<tr::audio::source> current_song;

	std::filesystem::path find_song_path(std::string_view name);
	std::optional<tr::audio::buffer> load_audio_file(const char* filename);
} // namespace engine

std::filesystem::path engine::find_song_path(std::string_view name)
{
	std::filesystem::path path{cli_settings.data_directory / "music" / TR_FMT::format("{}.ogg", name)};
	if (!std::filesystem::exists(path)) {
		path = cli_settings.user_directory / "music" / TR_FMT::format("{}.ogg", name);
		if (!std::filesystem::exists(path)) {
			return {};
		}
	}
	return path;
}

std::optional<tr::audio::buffer> engine::load_audio_file(const char* filename)
{
	try {
		const std::filesystem::path& path{cli_settings.data_directory / "sounds" / filename};
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
		for (int i = 0; i < int(sound::COUNT); ++i) {
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

std::vector<std::string> engine::create_available_song_list()
{
	std::vector<std::string> songs{"classic", "chonk", "swarm"};
	try {
		const std::filesystem::path userdir{engine::cli_settings.user_directory / "music"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{userdir}) {
			if (file.is_regular_file() && file.path().extension() == ".ogg") {
				songs.push_back(file.path().stem().string());
			}
		}
		LOG(tr::severity::INFO, "Created a list of {} available song(s).", songs.size());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "An error occurred while creating a list of available songs.");
		LOG_CONTINUE(err);
	}
	return songs;
}

void engine::play_sound(sound sound, float volume, float pan, float pitch)
{
	if (tr::audio::active() && sounds[int(sound)].has_value()) {
		std::optional<tr::audio::source> source{tr::audio::try_allocating_source(0)};
		if (source.has_value()) {
			source->use(*sounds[int(sound)]);
			source->set_classes(1);
			source->set_gain(volume * 0.75f);
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
			current_song->set_gain(0.6f, fade_in);
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