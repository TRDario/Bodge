#include "../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr std::uint8_t SETTINGS_VERSION{0};

////////////////////////////////////////////////////////////// CLI SETTINGS ///////////////////////////////////////////////////////////////

void cli_settings_t::parse(int argc, const char** argv)
{
	for (int i = 1; i < argc; ++i) {
		const std::string_view arg{argv[i]};

		if (arg == "--datadir" && ++i < argc) {
			datadir = argv[i];
		}
		else if (arg == "--userdir" && ++i < argc) {
			userdir = argv[i];
		}
		else if (arg == "--gamespeed" && ++i < argc) {
			std::from_chars(argv[i], argv[i] + std::strlen(argv[i]), game_speed);
		}
		else if (arg == "--debug") {
			debug_mode = true;
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

	if (datadir.empty()) {
		datadir = tr::executable_dir() / "data";
	}
	if (userdir.empty()) {
		userdir = tr::user_dir("TRDario", "Bodge");
	}

	constexpr std::array<const char*, 4> DIRECTORIES{"localization", "fonts", "replays", "gamemodes"};
	for (const char* directory : DIRECTORIES) {
		const std::filesystem::path path{userdir / directory};
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
		}
	}

	if (debug_mode) {
		logger = tr::logger{userdir / "log.txt"};
		tr::gfx_context::set_log_path(userdir / "gl.txt");
	}
}

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

void settings_t::raw_load_from_file() noexcept
{
	try {
		const std::filesystem::path path{cli_settings.userdir / "settings.dat"};
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		if (tr::binary_read<std::uint8_t>(data) != SETTINGS_VERSION) {
			LOG(tr::severity::ERROR, "Failed to load settings from '{}': Wrong settings file version.", path.string());
			return;
		}
		tr::binary_read(data, *this);
		LOG(tr::severity::INFO, "Loaded settings from '{}'.", (cli_settings.userdir / "settings.dat").string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load settings from '{}': {}.", (cli_settings.userdir / "settings.dat").string(), err.what());
	}
}

void settings_t::validate() noexcept
{
	if (window_size != FULLSCREEN) {
		const int clamped{std::clamp(window_size, MIN_WINDOW_SIZE, max_window_size())};
		if (clamped != window_size) {
			LOG(tr::severity::WARN, "Clamped window size from {} to {}.", window_size, clamped);
			window_size = clamped;
		}
	}
	if (refresh_rate != NATIVE_REFRESH_RATE) {
		const std::uint16_t clamped{std::clamp(refresh_rate, MIN_REFRESH_RATE, max_refresh_rate())};
		if (clamped != refresh_rate) {
			LOG(tr::severity::WARN, "Clamped refresh rate from {} to {}.", refresh_rate, clamped);
			refresh_rate = clamped;
		}
	}
	const std::uint8_t adjusted{std::clamp(msaa, NO_MSAA, tr::max_msaa())};
	if (adjusted != msaa) {
		LOG(tr::severity::WARN, "Adjusted MSAA from x{:d} to x{:d}.", msaa, adjusted);
		msaa = adjusted;
	}
	if (primary_hue >= 360) {
		const std::uint16_t clamped{static_cast<std::uint16_t>(primary_hue % 360)};
		LOG(tr::severity::WARN, "Clamped primary hue from {} to {}.", primary_hue, clamped);
		primary_hue = clamped;
	}
	if (secondary_hue >= 360) {
		const std::uint16_t clamped{static_cast<std::uint16_t>(secondary_hue % 360)};
		LOG(tr::severity::WARN, "Clamped secondary hue from {} to {}.", secondary_hue, clamped);
		secondary_hue = clamped;
	}
	if (sfx_volume > 100) {
		LOG(tr::severity::WARN, "Clamped SFX volume from {}% to 100%.", sfx_volume);
		sfx_volume = 100;
	}
	if (music_volume > 100) {
		LOG(tr::severity::WARN, "Clamped music volume from {}% to 100%.", music_volume);
		music_volume = 100;
	}
}

void settings_t::load_from_file() noexcept
{
	raw_load_from_file();
	validate();
}

void settings_t::save_to_file() noexcept
{
	const std::filesystem::path path{cli_settings.userdir / "settings.dat"};
	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, SETTINGS_VERSION);
		tr::binary_write(buffer, *this);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), tr::rand<std::uint8_t>(rng))};
		tr::binary_write(file, std::span{encrypted});
		LOG(tr::severity::INFO, "Saved settings to '{}'.", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save settings to '{}': {}", path.string(), err.what());
	}
}