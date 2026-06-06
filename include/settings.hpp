///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides settings structures and functionality.                                                                                       //
//                                                                                                                                       //
// Debug settings are parsed from command-line arguments on program start-up and are used for advanced settings that most players will   //
// never need to change.                                                                                                                 //
//                                                                                                                                       //
// Regular settings are loaded from and saved to <USER DIRECTORY>/settings.dat (a binary file) and are modifiable in-game.               //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "localization.hpp"

///////////////////////////////////////////////////////////// DEBUG SETTINGS //////////////////////////////////////////////////////////////

#ifdef TR_ENABLE_ASSERTS
#define BODGE_SHOW_PERF_DEFAULT 1
#else
#define BODGE_SHOW_PERF_DEFAULT 0
#endif

// Debug settings singleton.
class debug_settings {
  public:
	// Gets the debug settings instance.
	static debug_settings& instance();

	// Parses command-line arguments.
	tr::sys::signal parse(std::span<tr::cstring_view> args);
	// Validates command-line arguments.
	void validate();

	// Gets the path to the program data directory.
	const std::filesystem::path& data_directory() const;
	// Gets the path to the user data directory.
	const std::filesystem::path& user_directory() const;
	// Gets the rate at which frames are drawn.
	float refresh_rate() const;
	// Gets the speed multiplier of the game.
	float game_speed() const;
	// Gets whether the game speed is modified.
	bool modified_game_speed() const;
	// Gets whether to display performance statistics.
	bool show_performance_overlay() const;

  private:
	// Path to the program data directory.
	std::filesystem::path m_data_directory;
	// Path to the user data directory.
	std::filesystem::path m_user_directory;
	// Rate at which frames are drawn.
	float m_refresh_rate{+INFINITY};
	// Speed multiplier of the game.
	float m_game_speed{1.0f};
	// Whether to display performance statistics.
	bool m_show_perf{BODGE_SHOW_PERF_DEFAULT};

	// Constructs default command-line argumnt settings.
	debug_settings() = default;
};

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

// Application settings.
struct settings {
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
};

// Active application settings singleton class.
class active_settings {
  public:
	// Gets the active settings instance.
	static active_settings& instance();

	// Gets the active settings values.
	operator const settings&() const;
	// Gets the active settings values.
	const settings* operator->() const;

	// Determines whether releasing graphical resources ahead of applying settings is necessary.
	bool releasing_graphical_resources_required_to_apply(const settings& new_settings) const;

	// Applies new settings.
	void apply(const settings& new_settings);

  private:
	// The active settings structure.
	settings m_settings;

	// Loads the active settings from file.
	active_settings();
	// Saves the active settings to file.
	~active_settings();

	// Loads settings from a file without validation.
	void raw_load_from_file();
	// Validates loaded settings.
	void validate();

	// Determines whether a restart is necessary to apply new settings.
	bool restart_required_to_apply(const settings& new_settings) const;
};