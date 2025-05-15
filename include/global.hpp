#pragma once
#include <tr/audio.hpp>
#include <tr/graphics.hpp>
#include <tr/system.hpp>

using namespace tr::angle_literals;
using enum tr::align;

//////////////////////////////////////////////////////////// SETTINGS CONSTANTS ///////////////////////////////////////////////////////////

// Sentinel for a fullscreen window.
constexpr int FULLSCREEN{0};
// The minimum allowed window size.
constexpr int MIN_WINDOW_SIZE{250};
// The maximum supported window size.
int max_window_size() noexcept;

// Sentinel for a native refresh rate.
constexpr std::uint16_t NATIVE_REFRESH_RATE{0};
// The minimum allowed refresh rate.
constexpr std::uint16_t MIN_REFRESH_RATE{15};
// The maximum supported refresh rate.
std::uint16_t max_refresh_rate() noexcept;

// Sentinel for disabled multisampled anti-aliasing.
constexpr std::uint8_t NO_MSAA{0};
// The maximum allowed multisampled anti-aliasing.
std::uint8_t max_msaa() noexcept;

//////////////////////////////////////////////////////////// TICKRATE CONSTANTS ///////////////////////////////////////////////////////////

// Tick datatype.
using ticks = std::uint32_t;
// Game tickrate.
inline constexpr ticks SECOND_TICKS{240};
// Seconds -> Ticks literal.
constexpr ticks operator""_s(unsigned long long seconds) noexcept
{
	return 240 * seconds;
}
// Seconds -> Ticks literal.
constexpr ticks operator""_s(long double seconds) noexcept
{
	return 240 * seconds;
}

////////////////////////////////////////////////////////////// GAME CONSTANTS /////////////////////////////////////////////////////////////

// Game field border size.
inline constexpr float FIELD_BORDER{4};
// Game field minimum in-bounds position.
inline constexpr float FIELD_MIN{FIELD_BORDER};
// Game field maximum in-bounds position.
inline constexpr float FIELD_MAX{1000 - FIELD_BORDER};
// Invulnerability duration.
inline constexpr ticks PLAYER_INVULN_TIME{2_s};

/////////////////////////////////////////////////////////////////// RNG ///////////////////////////////////////////////////////////////////

// Global RNG.
inline tr::xorshiftr_128p rng;

////////////////////////////////////////////////////////////////// LOGGER /////////////////////////////////////////////////////////////////

// Global logger.
inline tr::logger logger;
// Logging macro.
#define LOG(...) TR_LOG(logger, __VA_ARGS__)

/////////////////////////////////////////////////////////////// LOCALIZATION //////////////////////////////////////////////////////////////

// Language code datatype.
using language_code = std::array<char, 2>;
// Global localization map.
inline tr::localization_map localization;
// Loads localization from file.
void load_localization() noexcept;

// Gets the name of the font preferred by a language.
std::string get_language_font(language_code code);