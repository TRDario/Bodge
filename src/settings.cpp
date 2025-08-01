#include "../include/settings.hpp"

template <> struct tr::binary_reader<settings> : tr::default_binary_reader<settings> {};
template <> struct tr::binary_writer<settings> : tr::default_binary_writer<settings> {};

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr std::uint8_t SETTINGS_VERSION{0};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

namespace engine {
	void raw_load_settings();
	void validate_settings();
} // namespace engine

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

void engine::parse_command_line(int argc, const char** argv)
{
	for (int i = 1; i < argc; ++i) {
		const std::string_view arg{argv[i]};

		if (arg == "--datadir" && ++i < argc) {
			cli_settings.datadir = argv[i];
		}
		else if (arg == "--userdir" && ++i < argc) {
			cli_settings.userdir = argv[i];
		}
		else if (arg == "--gamespeed" && ++i < argc) {
			std::from_chars(argv[i], argv[i] + std::strlen(argv[i]), cli_settings.game_speed);
		}
		else if (arg == "--debug") {
			cli_settings.debug_mode = true;
		}
		else if (arg == "--help") {
			std::cout << "Bodge v0.1.0 by TRDario, 2025.\n"
						 "Supported arguments:\n"
						 "--datadir <path>     - Redirects the game data directory.\n"
						 "--userdir <path>     - Redirects the game user directory.\n"
						 "--gamespeed <factor> - Sets the game speed multiplier.\n"
						 "--debug              - Enables debug mode.\n";
			std::exit(EXIT_SUCCESS);
		}
	}

	if (cli_settings.datadir.empty()) {
		cli_settings.datadir = tr::system::executable_dir() / "data";
	}
	if (cli_settings.userdir.empty()) {
		cli_settings.userdir = tr::system::user_dir();
	}

	constexpr std::array<const char*, 4> DIRECTORIES{"localization", "fonts", "replays", "gamemodes"};
	for (const char* directory : DIRECTORIES) {
		const std::filesystem::path path{cli_settings.userdir / directory};
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
		}
	}

	if (cli_settings.debug_mode) {
		tr::log = tr::logger{"tr", cli_settings.userdir / "tr.log"};
		tr::gfx::log = tr::logger{"gl", cli_settings.userdir / "gl.log"};
		logger = tr::logger{"Bodge", cli_settings.userdir / "Bodge.log"};
	}
}

void engine::raw_load_settings()
{
	const std::filesystem::path path{cli_settings.userdir / "settings.dat"};
	try {
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		if (tr::binary_read<std::uint8_t>(data) != SETTINGS_VERSION) {
			LOG(tr::severity::ERROR, "Failed to load settings.", path.string());
			LOG_CONTINUE("From: '{}'", path.string());
			LOG_CONTINUE("Wrong settings file version.");
			return;
		}
		tr::binary_read(data, settings);
		LOG(tr::severity::INFO, "Loaded settings.");
		LOG_CONTINUE("From: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load settings.", path.string());
		LOG_CONTINUE("From: '{}'", path.string());
		LOG_CONTINUE(err);
	}
}

void engine::validate_settings()
{
	if (settings.window_size != FULLSCREEN) {
		const int clamped{std::clamp(settings.window_size, MIN_WINDOW_SIZE, max_window_size())};
		if (clamped != settings.window_size) {
			LOG(tr::severity::WARN, "Clamped window size from {} to {}.", settings.window_size, clamped);
			settings.window_size = clamped;
		}
	}
	if (settings.refresh_rate != NATIVE_REFRESH_RATE) {
		const std::uint16_t clamped{std::clamp(settings.refresh_rate, MIN_REFRESH_RATE, max_refresh_rate())};
		if (clamped != settings.refresh_rate) {
			LOG(tr::severity::WARN, "Clamped refresh rate from {} to {}.", settings.refresh_rate, clamped);
			settings.refresh_rate = clamped;
		}
	}
	const std::uint8_t adjusted{std::clamp(settings.msaa, NO_MSAA, tr::system::max_msaa())};
	if (adjusted != settings.msaa) {
		LOG(tr::severity::WARN, "Adjusted MSAA from x{:d} to x{:d}.", settings.msaa, adjusted);
		settings.msaa = adjusted;
	}
	if (settings.primary_hue >= 360) {
		const std::uint16_t clamped{static_cast<std::uint16_t>(settings.primary_hue % 360)};
		LOG(tr::severity::WARN, "Clamped primary hue from {} to {}.", settings.primary_hue, clamped);
		settings.primary_hue = clamped;
	}
	if (settings.secondary_hue >= 360) {
		const std::uint16_t clamped{static_cast<std::uint16_t>(settings.secondary_hue % 360)};
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
	const std::filesystem::path path{cli_settings.userdir / "settings.dat"};
	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, SETTINGS_VERSION);
		tr::binary_write(buffer, settings);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), rng.generate<std::uint8_t>())};
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