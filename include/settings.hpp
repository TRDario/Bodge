///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides settings structures and functionality.                                                                                       //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "localization.hpp"

////////////////////////////////////////////////////////////// CLI SETTINGS ///////////////////////////////////////////////////////////////

// Settings passed by command-line arguments.
inline struct cli_settings {
	// Path to the program data directory.
	std::filesystem::path data_directory;
	// Path to the user data directory.
	std::filesystem::path user_directory;
	// The rate at which frames are drawn.
	float refresh_rate{+INFINITY};
	// The speed multiplier of the game.
	float game_speed{1.0f};
#ifdef TR_ENABLE_ASSERTS
	// Whether to display performance statistics.
	bool show_perf{true};
#else
	// Whether to display performance statistics.
	bool show_perf{false};
#endif
} g_cli_settings{}; // Global copy of the command-line settings.

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

// Minimum allowed window size.
constexpr u16 MIN_WINDOW_SIZE{500};
// Maximum allowed window size.
u16 max_window_size();

// Program display mode.
enum class display_mode : bool {
	WINDOWED,
	FULLSCREEN
};

// Sentinel denoting that multisampling is disabled.
constexpr u8 NO_MSAA{0};

// Program settings.
inline struct settings {
	// Size of the window (only used if display_mode == WINDOWED).
	u16 window_size{800};
	// Active display mode.
	display_mode display_mode{display_mode::WINDOWED};
	// Whether V-sync is enabled.
	bool vsync{false};
	// Mouse sensitivity as a percentage.
	u8 mouse_sensitivity{100};
	// Active player skin (or empty string for none).
	std::string player_skin;
	// Primary game hue.
	u16 primary_hue{60};
	// Secondary game hue.
	u16 secondary_hue{0};
	// Volume of the sound effects as a percentage.
	u8 sfx_volume{100};
	// Volume of the music as a percentage.
	u8 music_volume{100};
	// Active language.
	language_code language{'e', 'n'};

	bool operator==(const settings&) const = default;

	// Loads settings from a file.
	void load_from_file();
	// Saves settings to a file.
	void save_to_file() const;

	// Determines whether releasing graphical resources ahead of applying settings is necessary.
	bool releasing_graphical_resources_required_to_apply(const settings& new_settings) const;
	// Applies new settings.
	void apply(const settings& new_settings);

  private:
	// Loads settings from a file without validation.
	void raw_load_from_file();
	// Validates loaded settings.
	void validate();

	// Determines whether a restart is necessary to apply new settings.
	bool restart_required_to_apply(const settings& new_settings) const;
} g_settings{}; // Global copy of the settings.