#pragma once
#include "localization.hpp"

////////////////////////////////////////////////////////////// CLI SETTINGS ///////////////////////////////////////////////////////////////

constexpr float NATIVE_REFRESH_RATE{0};

struct cli_settings {
	std::filesystem::path data_directory;
	std::filesystem::path user_directory;
	float refresh_rate{NATIVE_REFRESH_RATE};
	float game_speed{1.0f};
#ifdef TR_ENABLE_ASSERTS
	bool debug_mode{true};
#else
	bool debug_mode{false};
#endif
};

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

constexpr std::uint16_t MIN_WINDOW_SIZE{500};
std::uint16_t max_window_size();

enum class display_mode : bool {
	WINDOWED,
	FULLSCREEN
};

constexpr std::uint8_t NO_MSAA{0};

struct settings {
	std::uint16_t window_size{500};
	display_mode display_mode{display_mode::WINDOWED};
	std::uint8_t msaa{0};
	std::uint16_t primary_hue{60};
	std::uint16_t secondary_hue{0};
	std::uint8_t sfx_volume{100};
	std::uint8_t music_volume{100};
	language_code language{'e', 'n'};

	bool operator==(const settings&) const = default;
};

///////////////////////////////////////////////////////////////// ENGINE //////////////////////////////////////////////////////////////////

namespace engine {
	inline cli_settings cli_settings{};
	void parse_command_line(int argc, const char** argv);

	inline settings settings{};
	void load_settings();
	void save_settings();
} // namespace engine