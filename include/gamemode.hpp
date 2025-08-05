#pragma once
#include "global.hpp"

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

// Gamemode player settings.
struct player_settings {
	// The number of starting lives.
	std::uint32_t starting_lives{2};
	// The size of the player hitbox.
	float hitbox_radius{10};
	// The player's movement inertia (range: [0, 1]).
	float inertia_factor{0.1f};

	bool operator==(const player_settings&) const = default;
};
template <> struct tr::binary_reader<player_settings> : tr::default_binary_reader<player_settings> {};
template <> struct tr::binary_writer<player_settings> : tr::default_binary_writer<player_settings> {};

// Sentinel for an autoplay gamemode.
constexpr player_settings NO_PLAYER{std::numeric_limits<std::uint32_t>::max()};

// Gets whether the gamemode is autoplay.
bool autoplay(const player_settings& ps);

////////////////////////////////////////////////////////////// BALL SETTINGS //////////////////////////////////////////////////////////////

// Gamemode ball settings.
struct ball_settings {
	// The starting number of balls.
	std::uint8_t starting_count{10};
	// The largest possible number of balls.
	std::uint8_t max_count{20};
	// The interval between ball spawns in ticks.
	ticks spawn_interval{10_s};
	// The initial size of ball spawns.
	float initial_size{20};
	// The amount added to the new ball size on every spawn.
	float size_step{1};
	// The initial velocity of ball spawns.
	float initial_velocity{300};
	// The amount added to the new ball velocity on every spawn.
	float velocity_step{20};

	bool operator==(const ball_settings&) const = default;
};
template <> struct tr::binary_reader<ball_settings> : tr::default_binary_reader<ball_settings> {};
template <> struct tr::binary_writer<ball_settings> : tr::default_binary_writer<ball_settings> {};

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

// Gamemode object.
struct gamemode {
	// Flag marking the gamemode as a special, built-in gamemode.
	bool builtin{false};
	// The name of the gamemode.
	std::string name{};
	// The author of the gamemode.
	std::string author{};
	// The description of the gamemode.
	std::string description{};
	// Player settings.
	player_settings player{};
	// Ball settings.
	ball_settings ball{};
	// The name of the song used for the gamemode.
	std::string song{};

	bool operator==(const gamemode&) const = default;
};
template <> struct tr::binary_reader<gamemode> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, gamemode& out);
};
template <> struct tr::binary_writer<gamemode> {
	static void write_to_stream(std::ostream& os, const gamemode& in);
};

// Randomly picks a menu gamemode.
gamemode pick_menu_gamemode();
// Loads a gamemode from file.
gamemode load_gamemode(const std::filesystem::path& path);
// Loads all found gamemodes.
std::vector<gamemode> load_gamemodes();
// Saves a gamemode to file.
void save_gamemode(const gamemode& gm);

// Gets the localization of the name of a gamemode.
std::string_view name(const gamemode& gm);
// Gets the localization of the description of a gamemode.
std::string_view description(const gamemode& gm);