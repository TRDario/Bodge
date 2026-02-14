#include "../include/audio.hpp"
#include "../include/settings.hpp"

///////////////////////////////////////////////////////////// INTERNAL HELPERS ////////////////////////////////////////////////////////////

// Tries to find a path to a song given a filename.
static std::filesystem::path try_finding_song_path(std::string_view name)
{
	std::filesystem::path path{g_cli_settings.data_directory / "music" / TR_FMT::format("{}.ogg", name)};
	if (!std::filesystem::exists(path)) {
		path = g_cli_settings.user_directory / "music" / TR_FMT::format("{}.ogg", name);
		if (!std::filesystem::exists(path)) {
			return {};
		}
	}
	return path;
}

// Tries to load an audio file.
static std::optional<tr::audio::buffer> try_loading_audio_file(const char* filename)
{
	try {
		return tr::audio::load_file(g_cli_settings.data_directory / "sounds" / filename);
	}
	catch (tr::audio::file_open_error&) {
		return std::nullopt;
	}
}

////////////////////////////////////////////////////////////////// AUDIO //////////////////////////////////////////////////////////////////

std::vector<std::string> create_available_song_list()
{
	std::vector<std::string> songs{"classic", "chonk", "swarm", "variety"};
	try {
		const std::filesystem::path userdir{g_cli_settings.user_directory / "music"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{userdir}) {
			if (file.is_regular_file() && file.path().extension() == ".ogg") {
				songs.push_back(file.path().stem().string());
			}
		}
	}
	catch (std::exception&) {
		return songs;
	}
	return songs;
}

void audio::initialize()
{
	constexpr std::array<const char*, int(sound::COUNT)> SFX_FILENAMES{
		"hover.ogg",          "hold.ogg",    "confirm.ogg",  "cancel.ogg",     "type.ogg",      "pause.ogg",
		"unpause.ogg",        "tick.ogg",    "tick_alt.ogg", "ball_spawn.ogg", "bounce.ogg",    "style.ogg",
		"fragment_spawn.ogg", "collect.ogg", "1up.ogg",      "hit.ogg",        "game_over.ogg",
	};

	try {
		tr::audio::initialize();
		tr::audio::set_master_gain(2);
		tr::audio::set_class_gain(0, g_settings.sfx_volume / 100.0f);
		tr::audio::set_class_gain(1, g_settings.music_volume / 100.0f);
		for (int i = 0; i < int(sound::COUNT); ++i) {
			m_sounds[i] = try_loading_audio_file(SFX_FILENAMES[i]);
		}
		m_current_song.emplace(1000);
		m_current_song->set_classes(2);
	}
	catch (tr::audio::init_error&) {
		return;
	}
}

void audio::apply_settings()
{
	if (tr::audio::active()) {
		tr::audio::set_class_gain(0, g_settings.sfx_volume / 100.0f);
		tr::audio::set_class_gain(1, g_settings.music_volume / 100.0f);
	}
}

void audio::shut_down()
{
	if (tr::audio::active()) {
		m_current_song.reset();
		for (std::optional<tr::audio::buffer>& sound : m_sounds) {
			sound.reset();
		}
		tr::audio::shut_down();
	}
}

//

void audio::play_sound(sound sound, float volume, float pan, float pitch)
{
	if (tr::audio::active() && m_sounds[int(sound)].has_value()) {
		std::optional<tr::audio::source> source{tr::audio::try_allocating_source(0)};
		if (source.has_value()) {
			source->use(*m_sounds[int(sound)]);
			source->set_classes(1);
			source->set_gain(volume * 0.75f);
			source->set_pitch(pitch);
			source->set_pos({tr::magth(1.0f, tr::acos(pan)), 0});
			source->set_rolloff(1.0f);
			source->play();
		}
	}
}

void audio::play_song(std::string_view name, tr::fsecs fade_in)
{
	play_song(name, 0s, fade_in);
}

void audio::play_song(std::string_view name, tr::fsecs offset, tr::fsecs fade_in)
{
	if (m_current_song.has_value()) {
		try {
			const std::filesystem::path& path{try_finding_song_path(name)};
			if (path.empty()) {
				return;
			}

			m_current_song->stop();
			m_current_song->use(tr::audio::open_file(path));
			m_current_song->set_offset(offset);
			m_current_song->set_pitch(1.0f);
			m_current_song->set_gain(0.25f);
			m_current_song->set_gain(0.6f, fade_in);
			m_current_song->play();
		}
		catch (tr::exception& err) {
			return;
		}
	}
}

void audio::set_song_speed(float multiplier)
{
	if (m_current_song.has_value()) {
		m_current_song->set_pitch(multiplier);
	}
}

void audio::pause_song()
{
	if (m_current_song.has_value()) {
		m_current_song->pause();
		m_current_song->set_gain(0);
	}
}

void audio::unpause_song()
{
	if (m_current_song.has_value()) {
		m_current_song->play();
		m_current_song->set_gain(0.75f, 0.5s);
	}
}

void audio::fade_song_out(tr::fsecs time)
{
	if (m_current_song.has_value()) {
		m_current_song->set_gain(0, time);
	}
}