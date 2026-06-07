///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements settings.hpp.                                                                                                              //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr u8 SETTINGS_VERSION{3};

////////////////////////////////////////////////////////////// DEBUG SETTINGS /////////////////////////////////////////////////////////////

debug_settings& debug_settings::instance()
{
	static debug_settings instance{};
	return instance;
}

//

tr::sys::signal debug_settings::parse(std::span<tr::cstring_view> args)
{
	for (auto arg_it = args.begin(); arg_it != args.end(); ++arg_it) {
		if (*arg_it == "--datadir" && ++arg_it < args.end()) {
			m_data_directory = std::filesystem::canonical(std::filesystem::path{*arg_it});
		}
		else if (*arg_it == "--userdir" && ++arg_it < args.end()) {
			const std::filesystem::path userdir_path{std::filesystem::path{*arg_it}};
			if (!std::filesystem::exists(userdir_path)) {
				std::filesystem::create_directory(userdir_path);
			}
			m_data_directory = std::filesystem::canonical(userdir_path);
		}
		else if (*arg_it == "--refreshrate" && ++arg_it < args.end()) {
			std::from_chars(*arg_it, *arg_it + std::strlen(*arg_it), m_refresh_rate);
		}
		else if (*arg_it == "--gamespeed" && ++arg_it < args.end()) {
			std::from_chars(*arg_it, *arg_it + std::strlen(*arg_it), m_game_speed);
		}
		else if (*arg_it == "--showperf") {
			m_show_perf = true;
		}
		else if (*arg_it == "--help") {
			std::cout << "Bodge " VERSION_STRING " by TRDario, 2025-2026.\n"
						 "Supported arguments:\n"
						 "--datadir <path>       - Overrides the data directory.\n"
						 "--userdir <path>       - Overrides the user directory.\n"
						 "--refreshrate <number> - Overrides the refresh rate.\n"
						 "--gamespeed <factor>   - Overrides the speed multiplier.\n"
						 "--showperf             - Shows performance information.\n";
			return tr::sys::signal::SUCCESS;
		}
	}
	return tr::sys::signal::CONTINUE;
}

void debug_settings::validate()
{
	if (m_data_directory.empty()) {
		m_data_directory = tr::sys::executable_dir() / "data";
	}
	if (m_user_directory.empty()) {
		m_user_directory = tr::sys::user_dir();
	}

	constexpr std::array<tr::cstring_view, 5> DIRECTORIES{"localization", "fonts", "music", "replays", "gamemodes"};
	for (tr::cstring_view directory : DIRECTORIES) {
		const std::filesystem::path path{m_user_directory / directory};
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
		}
	}

	m_refresh_rate = std::clamp(m_refresh_rate, 1.0f, tr::sys::refresh_rate());
}

//

const std::filesystem::path& debug_settings::data_directory() const
{
	return m_data_directory;
}

const std::filesystem::path& debug_settings::user_directory() const
{
	return m_user_directory;
}

float debug_settings::refresh_rate() const
{
	return m_refresh_rate;
}

float debug_settings::game_speed() const
{
	return m_game_speed;
}

bool debug_settings::modified_game_speed() const
{
	return m_game_speed != 1.0f;
}

bool debug_settings::show_performance_overlay() const
{
	return m_show_perf;
}

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

template <> struct tr::binary_reader<settings> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, settings& out)
	{
		span = tr::binary_read(span, out.window_size);
		span = tr::binary_read(span, out.display_mode);
		span = tr::binary_read(span, out.vsync);
		span = tr::binary_read(span, out.mouse_sensitivity);
		span = tr::binary_read(span, out.player_skin);
		span = tr::binary_read(span, out.primary_hue);
		span = tr::binary_read(span, out.secondary_hue);
		span = tr::binary_read(span, out.sfx_volume);
		span = tr::binary_read(span, out.music_volume);
		return tr::binary_read(span, out.language);
	}
};

template <> struct tr::binary_writer<settings> {
	static void write_to_stream(std::ostream& os, const settings& in)
	{
		tr::binary_write(os, in.window_size);
		tr::binary_write(os, in.display_mode);
		tr::binary_write(os, in.vsync);
		tr::binary_write(os, in.mouse_sensitivity);
		tr::binary_write(os, in.player_skin);
		tr::binary_write(os, in.primary_hue);
		tr::binary_write(os, in.secondary_hue);
		tr::binary_write(os, in.sfx_volume);
		tr::binary_write(os, in.music_volume);
		tr::binary_write(os, in.language);
	}
};

u16 max_window_size()
{
	const glm::ivec2 display_size{tr::sys::display_size()};
	return u16(std::min(display_size.x, display_size.y));
}

//

settings::settings(const std::filesystem::path& path)
{
	try {
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		if (tr::binary_read<u8>(data) == SETTINGS_VERSION) {
			tr::binary_read<::settings>(data, *this);
		}
	}
	catch (std::exception&) {
		return;
	}

	window_size = std::clamp(window_size, MIN_WINDOW_SIZE, max_window_size());
	mouse_sensitivity = std::clamp(mouse_sensitivity, 50_u8, 200_u8);
	primary_hue = u16(primary_hue % 360);
	secondary_hue = u16(secondary_hue % 360);
	sfx_volume = std::min(sfx_volume, 100_u8);
	music_volume = std::min(music_volume, 100_u8);
}

//

void settings::save_to_file(const std::filesystem::path& path) const
{
	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, SETTINGS_VERSION);
		tr::binary_write(buffer, *this);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), g_rng.generate<u8>())};
		tr::binary_write(file, std::span{encrypted});
	}
	catch (std::exception&) {
		return;
	}
}

//

bool settings::restart_required_to_apply(const settings& new_settings) const
{
	return new_settings.display_mode != display_mode || (display_mode == display_mode::WINDOWED && new_settings.window_size != window_size);
}