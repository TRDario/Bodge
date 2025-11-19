#pragma once
#include "localization.hpp"

////////////////////////////////////////////////////////////// CLI SETTINGS ///////////////////////////////////////////////////////////////

struct cli_settings {
	std::filesystem::path data_directory;
	std::filesystem::path user_directory;
	float refresh_rate{+INFINITY};
	float game_speed{1.0f};
#ifdef TR_ENABLE_ASSERTS
	bool show_perf{true};
#else
	bool show_perf{false};
#endif
};

inline cli_settings g_cli_settings{};

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

constexpr u16 MIN_WINDOW_SIZE{500};
u16 max_window_size();

enum class display_mode : bool {
	WINDOWED,
	FULLSCREEN
};

constexpr u8 NO_MSAA{0};

struct settings {
	u16 window_size{500};
	display_mode display_mode{display_mode::WINDOWED};
	bool vsync{false};
	u8 msaa{8};
	u16 primary_hue{60};
	u16 secondary_hue{0};
	u8 sfx_volume{100};
	u8 music_volume{100};
	language_code language{'e', 'n'};

	bool operator==(const settings&) const = default;

	void load_from_file();
	void save_to_file() const;

  private:
	void raw_load_from_file();
	void raw_load_v0(std::span<const std::byte> data);
	void validate();
};

inline settings g_settings{};