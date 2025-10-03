#include "../include/settings.hpp"
#include "../include/legacy_formats.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr u8 SETTINGS_VERSION{1};

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

template <> struct tr::binary_reader<settings> : tr::default_binary_reader<settings> {};
template <> struct tr::binary_writer<settings> : tr::default_binary_writer<settings> {};

namespace engine {
	void raw_load_settings();
	void raw_load_settings_v0(std::span<const std::byte> data);
	void validate_settings();
} // namespace engine

u16 max_window_size()
{
	const glm::ivec2 display_size{tr::sys::display_size()};
	return u16(std::min(display_size.x, display_size.y));
}

//

void engine::parse_command_line(int argc, const char** argv)
{
	for (int i = 1; i < argc; ++i) {
		const std::string_view arg{argv[i]};

		if (arg == "--datadir" && ++i < argc) {
			cli_settings.data_directory = argv[i];
		}
		else if (arg == "--userdir" && ++i < argc) {
			cli_settings.user_directory = argv[i];
		}
		else if (arg == "--refreshrate" && ++i < argc) {
			std::from_chars(argv[i], argv[i] + std::strlen(argv[i]), cli_settings.refresh_rate);
			cli_settings.refresh_rate = std::clamp(cli_settings.refresh_rate, 1.0f, tr::sys::refresh_rate());
		}
		else if (arg == "--gamespeed" && ++i < argc) {
			std::from_chars(argv[i], argv[i] + std::strlen(argv[i]), cli_settings.game_speed);
		}
		else if (arg == "--showperf") {
			cli_settings.show_perf = true;
		}
		else if (arg == "--help") {
			std::cout << "Bodge v1.1.2 by TRDario, 2025.\n"
						 "Supported arguments:\n"
						 "--datadir <path>       - Overrides the data directory.\n"
						 "--userdir <path>       - Overrides the user directory.\n"
						 "--refreshrate <number> - Overrides the refresh rate.\n"
						 "--gamespeed <factor>   - Overrides the speed multiplier.\n"
						 "--showperf             - Enables performance information.\n";
			std::exit(EXIT_SUCCESS);
		}
	}

	if (cli_settings.data_directory.empty()) {
		cli_settings.data_directory = tr::sys::executable_dir() / "data";
	}
	if (cli_settings.user_directory.empty()) {
		cli_settings.user_directory = tr::sys::user_dir();
	}
	if (cli_settings.refresh_rate == 0) {
		cli_settings.refresh_rate = tr::sys::refresh_rate();
	}

	constexpr std::array<const char*, 5> DIRECTORIES{"localization", "fonts", "music", "replays", "gamemodes"};
	for (const char* directory : DIRECTORIES) {
		const std::filesystem::path path{cli_settings.user_directory / directory};
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
		}
	}
}

void engine::raw_load_settings()
{
	const std::filesystem::path path{cli_settings.user_directory / "settings.dat"};
	try {
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		const u8 version{tr::binary_read<u8>(data)};
		switch (version) {
		case 0:
			raw_load_settings_v0(data);
			return;
		case 1:
			settings = tr::binary_read<::settings>(data);
			return;
		default:
			return;
		}
	}
	catch (std::exception&) {
		return;
	}
}

void engine::raw_load_settings_v0(std::span<const std::byte> data)
{
	const settings_v0 temp{tr::binary_read<settings_v0>(data)};
	settings.window_size = temp.window_size;
	settings.display_mode = temp.display_mode;
	settings.msaa = temp.msaa;
	settings.primary_hue = temp.primary_hue;
	settings.secondary_hue = temp.secondary_hue;
	settings.sfx_volume = temp.sfx_volume;
	settings.music_volume = temp.music_volume;
	settings.language = temp.language;
}

void engine::validate_settings()
{
	settings.window_size = std::clamp(settings.window_size, MIN_WINDOW_SIZE, max_window_size());
	settings.msaa = std::clamp(settings.msaa, NO_MSAA, tr::sys::max_msaa());
	settings.primary_hue = u16(settings.primary_hue % 360);
	settings.secondary_hue = u16(settings.secondary_hue % 360);
	settings.sfx_volume = std::min(settings.sfx_volume, 100_u8);
	settings.music_volume = std::min(settings.music_volume, 100_u8);
}

void engine::load_settings()
{
	raw_load_settings();
	validate_settings();
}

void engine::save_settings()
{
	try {
		std::ofstream file{tr::open_file_w(cli_settings.user_directory / "settings.dat", std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, SETTINGS_VERSION);
		tr::binary_write(buffer, settings);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), rng.generate<u8>())};
		tr::binary_write(file, std::span{encrypted});
	}
	catch (std::exception&) {
		return;
	}
}