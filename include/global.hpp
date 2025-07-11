#pragma once
#include <tr/audio.hpp>
#include <tr/sysgfx.hpp>

////////////////////////////////////////////////////////////////// USING //////////////////////////////////////////////////////////////////

using namespace tr::angle_literals;
using namespace tr::color_literals;

//////////////////////////////////////////////////////////// SETTINGS CONSTANTS ///////////////////////////////////////////////////////////

// Sentinel for a fullscreen window.
constexpr int FULLSCREEN{0};
// The minimum allowed window size.
constexpr int MIN_WINDOW_SIZE{500};
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

///////////////////////////////////////////////////////////////// TICKRATE ////////////////////////////////////////////////////////////////

// Tick datatype.
using ticks = std::uint32_t;
// Game tickrate.
inline constexpr ticks SECOND_TICKS{240};
// Seconds -> Ticks literal.
constexpr ticks operator""_s(unsigned long long seconds) noexcept
{
	return static_cast<ticks>(240 * seconds);
}
// Seconds -> Ticks literal.
constexpr ticks operator""_s(long double seconds) noexcept
{
	return static_cast<ticks>(240 * seconds);
}
// Seconds -> Ticks literal (float).
constexpr float operator""_sf(unsigned long long seconds) noexcept
{
	return static_cast<float>(240 * seconds);
}
// Seconds -> Ticks literal (float).
constexpr float operator""_sf(long double seconds) noexcept
{
	return static_cast<float>(240 * seconds);
}

// Formats timer text.
std::string timer_text(ticks time);

/////////////////////////////////////////////////////////// GRAPHICAL CONSTANTS ///////////////////////////////////////////////////////////

// Transformation matrix used by game elements.
inline const glm::mat4 TRANSFORM{tr::ortho(glm::vec2{1000.0f})};

// Overlay used to dim the game in the main menu.
constexpr std::array<glm::vec2, 4> OVERLAY_POSITIONS{{{0, 0}, {1000, 0}, {1000, 1000}, {0, 1000}}};
// Tint used for the menu game overlay.
constexpr tr::rgba8 MENU_GAME_OVERLAY_TINT{0, 0, 0, 160};

////////////////////////////////////////////////////////////// GAME CONSTANTS /////////////////////////////////////////////////////////////

// Game field border size.
inline constexpr float FIELD_BORDER{4};
// Game field minimum in-bounds position.
inline constexpr float FIELD_MIN{FIELD_BORDER};
// Game field maximum in-bounds position.
inline constexpr float FIELD_MAX{1000 - FIELD_BORDER};
// Invulnerability duration.
inline constexpr ticks PLAYER_INVULN_TIME{2_s};

/////////////////////////////////////////////////////////////// UI CONSTANTS //////////////////////////////////////////////////////////////

// Starting position for elements on the top of the screen.
constexpr glm::vec2 TOP_START_POS{500, -50};
// Starting position for elements on the bottom of the screen.
constexpr glm::vec2 BOTTOM_START_POS{500, 1050};

// Final position for screen titles.
constexpr glm::vec2 TITLE_POS{500, 0};

/////////////////////////////////////////////////////////////////// RNG ///////////////////////////////////////////////////////////////////

// Global RNG.
inline tr::xorshiftr_128p rng;

////////////////////////////////////////////////////////////////// LOGGER /////////////////////////////////////////////////////////////////

// Global logger.
inline tr::logger logger;
// Logging macro.
#define LOG(...) TR_LOG(logger, __VA_ARGS__)
// Logging macro.
#define LOG_CONTINUE(...) TR_LOG_CONTINUE(logger, __VA_ARGS__)

/////////////////////////////////////////////////////////////////// TIME //////////////////////////////////////////////////////////////////

// Gets the current unix timestamp.
std::int64_t unix_now() noexcept;

/////////////////////////////////////////////////////////////////// TIME //////////////////////////////////////////////////////////////////

// Struct containing information about a label widget.
struct label {
	// The tag of the label.
	const char* tag;
	// The tooltip of the label.
	const char* tooltip;
};