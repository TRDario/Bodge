#pragma once
#include "localization.hpp"

////////////////////////////////////////////////////////////// CLI SETTINGS ///////////////////////////////////////////////////////////////

// Sentinel for a native refresh rate.
constexpr float NATIVE_REFRESH_RATE{0};

// Settings gotten from command-line arguments.
struct cli_settings {
	// The path to the data directroy.
	std::filesystem::path datadir;
	// The path to the save directory.
	std::filesystem::path userdir;
	// The refresh rate of the game.
	float refresh_rate{NATIVE_REFRESH_RATE};
	// The game speed factor.
	float game_speed{1.0f};
#ifdef TR_ENABLE_ASSERTS
	// Whether debug mode should be enabled.
	bool debug_mode{true};
#else
	// Whether debug mode should be enabled.
	bool debug_mode{false};
#endif
};

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

// The minimum allowed window size.
constexpr std::uint16_t MIN_WINDOW_SIZE{500};
// The maximum supported window size.
std::uint16_t max_window_size();

// Game display modes.
enum class display_mode : bool {
	// Game is displayed in a window.
	WINDOWED,
	// Game is displayed fullscreen.
	FULLSCREEN
};

// Sentinel for disabled multisampled anti-aliasing.
constexpr std::uint8_t NO_MSAA{0};

// Main program settings.
struct settings {
	// The size of the window in pixels if the game is run in windowed mode.
	std::uint16_t window_size{500};
	// Whether to display the game in fullscreen.
	display_mode display_mode{display_mode::WINDOWED};
	// The number of MSAA samples to use.
	std::uint8_t msaa{0};
	// The language to use.
	language_code language{'e', 'n'};

	// Hue used primarily for the player and borders.
	std::uint16_t primary_hue{60};
	// Hue used primarily for the balls.
	std::uint16_t secondary_hue{0};

	// The volume of the sound effects as a percentage.
	std::uint8_t sfx_volume{100};
	// The volume of the music as a percentage.
	std::uint8_t music_volume{100};

	bool operator==(const settings&) const = default;
};

///////////////////////////////////////////////////////////////// ENGINE //////////////////////////////////////////////////////////////////

namespace engine {
	// Global instance of the command-line settings.
	inline cli_settings cli_settings{};
	// Parses CLI settings from the argument list.
	void parse_command_line(int argc, const char** argv);

	// Currently active settings.
	inline settings settings{};
	// Loads settings from file.
	void load_settings();
	// Saves settings to file.
	void save_settings();
} // namespace engine