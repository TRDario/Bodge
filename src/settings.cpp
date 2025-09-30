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
	void raw_load_settings_v0(const std::filesystem::path& path, std::span<const std::byte> data);
	void raw_load_settings_v1(const std::filesystem::path& path, std::span<const std::byte> data);
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
			cli_settings.show_fps = true;
		}
		else if (arg == "--help") {
			std::cout << "Bodge v0.1.0 by TRDario, 2025.\n"
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

#ifdef TR_ENABLE_ASSERTS
	tr::log = tr::logger{"tr", cli_settings.user_directory / "tr.log"};
	tr::gfx::log = tr::logger{"gl", cli_settings.user_directory / "gl.log"};
	logger = tr::logger{"Bodge", cli_settings.user_directory / "Bodge.log"};
#endif
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
			raw_load_settings_v0(path, data);
			return;
		case 1:
			raw_load_settings_v1(path, data);
			return;
		default:
			LOG(tr::severity::ERROR, "Failed to load settings.");
			LOG_CONTINUE("From: '{}'", path.string());
			LOG_CONTINUE("Unsupported settings file version {:d}.", version);
			return;
		}
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load settings.", path.string());
		LOG_CONTINUE("From: '{}'", path.string());
		LOG_CONTINUE(err);
	}
}

void engine::raw_load_settings_v0(const std::filesystem::path& path, std::span<const std::byte> data)
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
	LOG(tr::severity::INFO, "Converted legacy settings file (v0) to latest format.");
	LOG_CONTINUE("From: '{}'", path.string());
}

void engine::raw_load_settings_v1(const std::filesystem::path& path, std::span<const std::byte> data)
{
	settings = tr::binary_read<::settings>(data);
	LOG(tr::severity::INFO, "Loaded settings.");
	LOG_CONTINUE("From: '{}'", path.string());
}

void engine::validate_settings()
{
	const int clamped_window_size{std::clamp(settings.window_size, MIN_WINDOW_SIZE, max_window_size())};
	if (clamped_window_size != settings.window_size) {
		LOG(tr::severity::WARN, "Clamped window size from {} to {}.", settings.window_size, clamped_window_size);
		settings.window_size = clamped_window_size;
	}
	const u8 adjusted{std::clamp(settings.msaa, NO_MSAA, tr::sys::max_msaa())};
	if (adjusted != settings.msaa) {
		LOG(tr::severity::WARN, "Adjusted MSAA from x{:d} to x{:d}.", settings.msaa, adjusted);
		settings.msaa = adjusted;
	}
	if (settings.primary_hue >= 360) {
		const u16 clamped{u16(settings.primary_hue % 360)};
		LOG(tr::severity::WARN, "Clamped primary hue from {} to {}.", settings.primary_hue, clamped);
		settings.primary_hue = clamped;
	}
	if (settings.secondary_hue >= 360) {
		const u16 clamped{u16(settings.secondary_hue % 360)};
		LOG(tr::severity::WARN, "Clamped secondary hue from {} to {}.", settings.secondary_hue, clamped);
		settings.secondary_hue = clamped;
	}
	if (settings.sfx_volume > 100) {
		LOG(tr::severity::WARN, "Clamped SFX volume from {}% to 100%.", settings.sfx_volume);
		settings.sfx_volume = 100;
	}
	if (settings.music_volume > 100) {
		LOG(tr::severity::WARN, "Clamped music volume from {}% to 100%.", settings.music_volume);
		settings.music_volume = 100;
	}
}

void engine::load_settings()
{
	raw_load_settings();
	validate_settings();
}

void engine::save_settings()
{
	const std::filesystem::path path{cli_settings.user_directory / "settings.dat"};
	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, SETTINGS_VERSION);
		tr::binary_write(buffer, settings);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), rng.generate<u8>())};
		tr::binary_write(file, std::span{encrypted});
		LOG(tr::severity::INFO, "Saved settings.");
		LOG_CONTINUE("To: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save settings.");
		LOG_CONTINUE("To: '{}'", path.string());
		LOG_CONTINUE(err);
	}
}