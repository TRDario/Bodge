#pragma once
#include <tr/audio.hpp>
#include <tr/graphics.hpp>
#include <tr/system.hpp>

///////////////////////////////////////////////////////////////// ALIASES /////////////////////////////////////////////////////////////////

using namespace tr::angle_literals;
namespace rs = std::ranges;
namespace vs = std::views;
namespace ch = std::chrono;
using enum tr::align;
using enum tr::ttf_style;
using enum tr::severity;
using mods = tr::keymods;
using key = tr::keycode;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using glm::vec2;
using std::abs;
using std::array;
using std::byte;
using std::clamp;
using std::format;
using std::function;
using std::ifstream;
using std::istream;
using std::list;
using std::make_unique;
using std::map;
using std::max;
using std::min;
using std::ofstream;
using std::optional;
using std::ostream;
using std::pair;
using std::pow;
using std::size_t;
using std::span;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::vector;
using std::filesystem::directory_entry;
using std::filesystem::directory_iterator;
using std::filesystem::path;
using tr::align;
using tr::audio_buffer;
using tr::audio_source;
using tr::binary_read;
using tr::binary_write;
using tr::bitmap;
using tr::color_alloc;
using tr::color_mesh_alloc;
using tr::decrypt;
using tr::encrypt;
using tr::encrypt_to;
using tr::fangle;
using tr::fill_poly_idx;
using tr::fill_poly_outline_idx;
using tr::fill_rect_outline_vtx;
using tr::fill_rect_vtx;
using tr::flush_binary;
using tr::frect2;
using tr::halign;
using tr::norm_cast;
using tr::open_file_r;
using tr::open_file_w;
using tr::poly_idx;
using tr::poly_outline_idx;
using tr::range_bytes;
using tr::rgba8;
using tr::state;
using tr::static_string;
using tr::static_vector;
using tr::tex_alloc;
using tr::tex_mesh_alloc;
using tr::texture;
using tr::texture_unit;
using tr::ttf_style;
using tr::ttfont;
using tr::UNLIMITED_WIDTH;

//////////////////////////////////////////////////////////// SETTINGS CONSTANTS ///////////////////////////////////////////////////////////

// Sentinel for a fullscreen window.
constexpr int FULLSCREEN{0};
// The minimum allowed window size.
constexpr int MIN_WINDOW_SIZE{500};
// The maximum supported window size.
int max_window_size() noexcept;

// Sentinel for a native refresh rate.
constexpr u16 NATIVE_REFRESH_RATE{0};
// The minimum allowed refresh rate.
constexpr u16 MIN_REFRESH_RATE{15};
// The maximum supported refresh rate.
u16 max_refresh_rate() noexcept;

// Sentinel for disabled multisampled anti-aliasing.
constexpr u8 NO_MSAA{0};
// The maximum allowed multisampled anti-aliasing.
u8 max_msaa() noexcept;

//////////////////////////////////////////////////////////// TICKRATE CONSTANTS ///////////////////////////////////////////////////////////

// Tick datatype.
using ticks = u32;
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
// Seconds -> Ticks literal (float).
constexpr float operator""_sf(unsigned long long seconds) noexcept
{
	return 240 * seconds;
}
// Seconds -> Ticks literal (float).
constexpr float operator""_sf(long double seconds) noexcept
{
	return 240 * seconds;
}

/////////////////////////////////////////////////////////// GRAPHICAL CONSTANTS ///////////////////////////////////////////////////////////

// Transformation matrix used by game elements.
inline const glm::mat4 TRANSFORM{tr::ortho(vec2{1000.0f})};

// Overlay used to dim the game in the main menu.
constexpr array<vec2, 4> OVERLAY_POSITIONS{{{0, 0}, {1000, 0}, {1000, 1000}, {0, 1000}}};
// Tint used for the menu game overlay.
constexpr rgba8 MENU_GAME_OVERLAY_TINT{0, 0, 0, 160};

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