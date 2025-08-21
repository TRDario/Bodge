#pragma once
#include <tr/audio.hpp>
#include <tr/sysgfx.hpp>

// Bodge version string.
constexpr const char* VERSION_STRING{"v0.9.0b"};

////////////////////////////////////////////////////////////////// USING //////////////////////////////////////////////////////////////////

using namespace std::chrono_literals;
using namespace tr::angle_literals;
using namespace tr::color_literals;

///////////////////////////////////////////////////////////////// TICKRATE ////////////////////////////////////////////////////////////////

// Tick datatype.
using ticks = std::uint32_t;
// Game tickrate.
inline constexpr ticks SECOND_TICKS{240};
// Seconds -> Ticks literal.
constexpr ticks operator""_s(unsigned long long seconds)
{
	return ticks(SECOND_TICKS * seconds);
}
// Seconds -> Ticks literal.
constexpr ticks operator""_s(long double seconds)
{
	return ticks(SECOND_TICKS * seconds);
}
// Seconds -> Ticks literal (float).
constexpr float operator""_sf(unsigned long long seconds)
{
	return float(SECOND_TICKS * seconds);
}
// Seconds -> Ticks literal (float).
constexpr float operator""_sf(long double seconds)
{
	return float(SECOND_TICKS * seconds);
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

/////////////////////////////////////////////////////////////////// TIME //////////////////////////////////////////////////////////////////

// Gets the current unix timestamp.
std::int64_t unix_now();

////////////////////////////////////////////////////////////////// LABEL //////////////////////////////////////////////////////////////////

// Widget tag type.
using tag = const char*;
// Struct containing information about a label widget.
struct label_info {
	// The tag of the label.
	tag tag;
	// The tooltip of the label.
	const char* tooltip;
};

////////////////////////////////////////////////////////////////// ENGINE /////////////////////////////////////////////////////////////////

namespace engine {
	// Global RNG.
	inline tr::xorshiftr_128p rng;
	// Global logger.
	inline tr::logger logger;
// Logging macro.
#define LOG(...) TR_LOG(::engine::logger, __VA_ARGS__)
// Logging macro.
#define LOG_CONTINUE(...) TR_LOG_CONTINUE(::engine::logger, __VA_ARGS__)

} // namespace engine