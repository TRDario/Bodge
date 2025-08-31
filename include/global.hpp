#pragma once
#include <charconv>
#include <tr/audio.hpp>
#include <tr/sysgfx.hpp>

// Bodge version string.
constexpr const char* VERSION_STRING{"v1.1.0"};

////////////////////////////////////////////////////////////////// USING //////////////////////////////////////////////////////////////////

using namespace std::chrono_literals;
using namespace tr::angle_literals;
using namespace tr::color_literals;
using namespace tr::integer_literals;
using namespace tr::integer_aliases;

///////////////////////////////////////////////////////////////// TICKRATE ////////////////////////////////////////////////////////////////

using ticks = u32;
inline constexpr ticks SECOND_TICKS{240};
constexpr ticks operator""_s(unsigned long long seconds)
{
	return ticks(SECOND_TICKS * seconds);
}
constexpr ticks operator""_s(long double seconds)
{
	return ticks(SECOND_TICKS * seconds);
}
constexpr float operator""_sf(unsigned long long seconds)
{
	return float(SECOND_TICKS * seconds);
}
constexpr float operator""_sf(long double seconds)
{
	return float(SECOND_TICKS * seconds);
}

// Sentinel for an inactive timer.
inline constexpr ticks INACTIVE_TIMER{~0U};

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

inline constexpr float FIELD_BORDER_THICKNESS{4};
inline constexpr float FIELD_MIN{FIELD_BORDER_THICKNESS};
inline constexpr float FIELD_MAX{1000 - FIELD_BORDER_THICKNESS};
inline constexpr float FIELD_CENTER{FIELD_MIN + (FIELD_MAX - FIELD_MIN) / 2};

/////////////////////////////////////////////////////////////// UI CONSTANTS //////////////////////////////////////////////////////////////

// Starting position for elements on the top of the screen.
constexpr glm::vec2 TOP_START_POS{500, -50};
// Starting position for elements on the bottom of the screen.
constexpr glm::vec2 BOTTOM_START_POS{500, 1050};

// Final position for screen titles.
constexpr glm::vec2 TITLE_POS{500, 0};

/////////////////////////////////////////////////////////////////// TIME //////////////////////////////////////////////////////////////////

i64 current_timestamp();

////////////////////////////////////////////////////////////////// LABEL //////////////////////////////////////////////////////////////////

using tag = const char*;

struct label_info {
	tag tag;
	const char* tooltip;
};

////////////////////////////////////////////////////////////////// ENGINE /////////////////////////////////////////////////////////////////

namespace engine {
	inline tr::xorshiftr_128p rng;
	inline tr::logger logger;
} // namespace engine

#define LOG(...) TR_LOG(::engine::logger, __VA_ARGS__)
#define LOG_CONTINUE(...) TR_LOG_CONTINUE(::engine::logger, __VA_ARGS__)

//

// Fragment used in some animations.
struct fragment {
	glm::vec2 pos;
	glm::vec2 vel;
	tr::angle rot;
	tr::angle rotvel;

	void update();
};