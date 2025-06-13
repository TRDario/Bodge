#include "../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr u8 SETTINGS_VERSION{0};

////////////////////////////////////////////////////////////// CLI SETTINGS ///////////////////////////////////////////////////////////////

void cli_settings_t::parse(int argc, const char** argv)
{
	for (int i = 1; i < argc; ++i) {
		const string_view arg{argv[i]};

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

	constexpr array<const char*, 4> DIRECTORIES{"localization", "fonts", "replays", "gamemodes"};
	for (const char* directory : DIRECTORIES) {
		const path path{userdir / directory};
		if (!is_directory(path)) {
			create_directory(path);
		}
	}

	if (debug_mode) {
		logger = tr::logger{userdir / "log.txt"};
	}
}

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

void settings_t::raw_load_from_file() noexcept
{
	try {
		const path path{cli_settings.userdir / "settings.dat"};
		ifstream file{open_file_r(path, std::ios::binary)};
		const vector<byte> raw{decrypt(flush_binary(file))};
		span<const byte> data{raw};
		if (binary_read<u8>(data) != SETTINGS_VERSION) {
			LOG(ERROR, "Failed to load settings from '{}': Wrong settings file version.", path.string());
			return;
		}
		binary_read(data, *this);
		LOG(INFO, "Loaded settings from '{}'.", (cli_settings.userdir / "settings.dat").string());
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to load settings from '{}': {}.", (cli_settings.userdir / "settings.dat").string(), err.what());
	}
}

void settings_t::validate() noexcept
{
	if (window_size != FULLSCREEN) {
		const int clamped{clamp(window_size, MIN_WINDOW_SIZE, max_window_size())};
		if (clamped != window_size) {
			LOG(WARN, "Clamped window size from {} to {}.", window_size, clamped);
			window_size = clamped;
		}
	}
	if (refresh_rate != NATIVE_REFRESH_RATE) {
		const u16 clamped{clamp(refresh_rate, MIN_REFRESH_RATE, max_refresh_rate())};
		if (clamped != refresh_rate) {
			LOG(WARN, "Clamped refresh rate from {} to {}.", refresh_rate, clamped);
			refresh_rate = clamped;
		}
	}
	const u8 adjusted{clamp(msaa, NO_MSAA, max_msaa())};
	if (adjusted != msaa) {
		LOG(WARN, "Adjusted MSAA from x{:d} to x{:d}.", msaa, adjusted);
		msaa = adjusted;
	}
	if (primary_hue >= 360) {
		const u16 clamped{static_cast<u16>(primary_hue % 360)};
		LOG(WARN, "Clamped primary hue from {} to {}.", primary_hue, clamped);
		primary_hue = clamped;
	}
	if (secondary_hue >= 360) {
		const u16 clamped{static_cast<u16>(secondary_hue % 360)};
		LOG(WARN, "Clamped secondary hue from {} to {}.", secondary_hue, clamped);
		secondary_hue = clamped;
	}
	if (sfx_volume > 100) {
		LOG(WARN, "Clamped SFX volume from {}% to 100%.", sfx_volume);
		sfx_volume = 100;
	}
	if (music_volume > 100) {
		LOG(WARN, "Clamped music volume from {}% to 100%.", music_volume);
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
	const path path{cli_settings.userdir / "settings.dat"};
	try {
		ofstream file{open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		binary_write(buffer, SETTINGS_VERSION);
		binary_write(buffer, *this);
		const vector<byte> encrypted{encrypt(range_bytes(buffer.view()), rand<u8>(rng))};
		binary_write(file, span{encrypted});
		LOG(INFO, "Saved settings to '{}'.", path.string());
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to save settings to '{}': {}", path.string(), err.what());
	}
}