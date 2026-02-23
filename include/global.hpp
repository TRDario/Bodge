///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides various functionality used throughout the codebase.                                                                          //
//                                                                                                                                       //
// Globals:                                                                                                                              //
//  • g_audio              - Audio subsystem.                                                                                            //
//  • g_cli_settings       - Command-line settings.                                                                                      //
//  • g_held_buttons       - Currently held mouse buttons.                                                                               //
//  • g_held_keymods       - Currently held keyboard modifiers.                                                                          //
//  • g_languages          - List of available languages.                                                                                //
//  • g_loaded_userdata    - Flag denoting whether userdata globals (scorefile, settings) have been loaded.                              //
//  • g_loc                - Localization map.                                                                                           //
//  • g_mouse_pos          - Current mouse position in normalized screen coordinates.                                                    //
//  • g_new_gamemode_draft - Saved draft of an unfinished new gamemode.                                                                  //
//  • g_renderer           - Rendering subsystem.                                                                                        //
//  • g_rng                - Global RNG (games use their own RNG for gameplay).                                                          //
//  • g_scorefile          - Player scorefile.                                                                                           //
//  • g_settings           - Game settings.                                                                                              //
//  • g_state_machine      - Container for the current state.                                                                            //
//  • g_text_engine        - Text rendering subsystem.                                                                                   //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <tr/audio.hpp>   // IWYU pragma: export
#include <tr/sysgfx.hpp>  // IWYU pragma: export
#include <tr/utility.hpp> // IWYU pragma: export

////////////////////////////////////////////////////////////////// USING //////////////////////////////////////////////////////////////////

using namespace std::chrono_literals;
using namespace tr::angle_literals;
using namespace tr::color_literals;
using namespace tr::integer_literals;
using namespace tr::integer_aliases;
using namespace tr::matrix_operators;
using namespace tr::sys::keyboard_literals;

///////////////////////////////////////////////////////////////// TICKRATE ////////////////////////////////////////////////////////////////

// Atomic unit of time used by the game simulation and UI.
using ticks = u32;
// Number of ticks in a second.
inline constexpr ticks SECOND_TICKS{240};
// Converts a number of seconds into a value in ticks.
consteval ticks operator""_s(unsigned long long seconds);
// Converts a number of seconds into a value in ticks.
consteval ticks operator""_s(long double seconds);
// Converts a number of seconds into a value in fractional ticks.
consteval float operator""_sf(unsigned long long seconds);
// Converts a number of seconds into a value in fractional ticks.
consteval float operator""_sf(long double seconds);
// Converts a number of beats given a BPM to a value in ticks.
consteval ticks beats_bpm(int beats, int bpm);

// Formats a score.
std::string format_score(i64 score);
// Formats a time in ticks to (MM):SS:mm.
std::string format_time(ticks time);
// Formats a time in ticks to MM:SS:mm.
std::string format_time_long(ticks time);
// Formats a playtime in ticks to HH:MM:SS.
std::string format_playtime(ticks playtime);
// Formats a timestamp to YY/MM/DD HH:MM.
std::string format_timestamp(i64 timestamp);

/////////////////////////////////////////////////////////// GRAPHICAL CONSTANTS ///////////////////////////////////////////////////////////

// Commonly-used yellow.
inline constexpr tr::rgba8 YELLOW{"FFFF00C0"_rgba8};
// Commonly-used shade of gray for held widgets.
inline constexpr tr::rgba8 HELD_GRAY{"202020"_rgba8};
// Commonly-used shade of gray for disabled widgets.
inline constexpr tr::rgba8 DISABLED_GRAY{"505050A0"_rgba8};
// Commonly-used darker shade of gray.
inline constexpr tr::rgba8 DARK_GRAY{"80808080"_rgba8};
// Commonly-used shade of gray.
inline constexpr tr::rgba8 GRAY{"A0A0A0A0"_rgba8};
// Commonly-used white.
inline constexpr tr::rgba8 WHITE{"FFFFFF"_rgba8};

// Transformation matrix used by game elements.
inline const glm::mat4 TRANSFORM{tr::ortho(glm::vec2{1000.0f})};

// Overlay used to dim the game in the main menu.
inline constexpr std::array<glm::vec2, 4> OVERLAY_POSITIONS{{{0, 0}, {1000, 0}, {1000, 1000}, {0, 1000}}};
// Tint used for the menu game overlay.
inline constexpr tr::rgba8 MENU_GAME_OVERLAY_TINT{0, 0, 0, 160};

////////////////////////////////////////////////////////////// GAME CONSTANTS /////////////////////////////////////////////////////////////

// Thickness of the field border.
inline constexpr float FIELD_BORDER_THICKNESS{4};
// Minimum coordinate of the field.
inline constexpr float FIELD_MIN{FIELD_BORDER_THICKNESS};
// Maximum coordinate fo the field.
inline constexpr float FIELD_MAX{1000 - FIELD_BORDER_THICKNESS};

/////////////////////////////////////////////////////////////// UI CONSTANTS //////////////////////////////////////////////////////////////

// Starting position for elements on the top of the screen.
inline constexpr glm::vec2 TOP_START_POS{500, -50};
// Starting position for elements on the bottom of the screen.
inline constexpr glm::vec2 BOTTOM_START_POS{500, 1050};

// Final position for screen titles.
inline constexpr glm::vec2 TITLE_POS{500, 0};

////////////////////////////////////////////////////////////////// INPUT //////////////////////////////////////////////////////////////////

// The held keyboard modifiers.
inline tr::sys::keymod g_held_keymods{tr::sys::keymod::NONE};
// The position of the mouse.
inline glm::vec2 g_mouse_pos{500, 500};
// The held mouse buttons.
inline tr::sys::mouse_button g_held_buttons{};

// Chooses one of three values based on the currently held keymods.
template <class T> T keymods_choose(T min, T mid, T max);

////////////////////////////////////////////////////////////// MISCELLANEOUS //////////////////////////////////////////////////////////////

// Fragment used in some animations.
struct fragment {
	// Position of the fragment.
	glm::vec2 pos;
	// Velocity of the fragment.
	glm::vec2 vel;
	// Rotation of the fragment.
	tr::angle rot;
	// Angular velocity of the fragment.
	tr::angle rotvel;

	// Updates the fragment.
	void tick();
};

// Flag denoting whether userdata globals (scorefile, settings) have been loaded.
inline bool g_loaded_userdata{false};
// The global RNG.
inline tr::xorshiftr_128p g_rng;

// Gets the current UNIX timestamp.
i64 current_timestamp();

// Opens the game window.
void open_window();

////////////////////////////////////////////////////////////// IMPLEMENTAION //////////////////////////////////////////////////////////////

consteval ticks operator""_s(unsigned long long seconds)
{
	return ticks(SECOND_TICKS * seconds);
}

consteval ticks operator""_s(long double seconds)
{
	return ticks(SECOND_TICKS * seconds);
}

consteval float operator""_sf(unsigned long long seconds)
{
	return float(SECOND_TICKS * seconds);
}

consteval float operator""_sf(long double seconds)
{
	return float(SECOND_TICKS * seconds);
}

consteval ticks beats_bpm(int beats, int bpm)
{
	return beats * 60_s / bpm;
}

//

template <class T> T keymods_choose(T min, T mid, T max)
{
	if (g_held_keymods & tr::sys::keymod::CTRL) {
		return max;
	}
	else if (g_held_keymods & tr::sys::keymod::SHIFT) {
		return mid;
	}
	else {
		return min;
	}
}