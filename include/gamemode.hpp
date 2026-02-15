#pragma once
#include "global.hpp"

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

// Player-related gamemode settings.
struct player_settings {
	// The starting number of lives.
	u8 starting_lives{2};
	// Whether to spawn life fragments.
	bool spawn_life_fragments{true};
	// The interval between life fragment spawning events.
	ticks life_fragment_spawn_interval{45_s};
	// The radius of the player's hitbox.
	float hitbox_radius{10};
	// The player's movement inertia factor.
	float inertia_factor{0.1f};

	bool operator==(const player_settings&) const = default;
};
template <> struct tr::binary_reader<player_settings> : tr::default_binary_reader<player_settings> {};
template <> struct tr::binary_writer<player_settings> : tr::default_binary_writer<player_settings> {};

////////////////////////////////////////////////////////////// BALL SETTINGS //////////////////////////////////////////////////////////////

// Ball-related gamemode settings.
struct ball_settings {
	// The starting number of balls.
	u8 starting_count{10};
	// The maximum number of balls.
	u8 max_count{20};
	// The interval between ball spawning events.
	ticks spawn_interval{10_s};
	// The initial size of spawned balls.
	float initial_size{20};
	// Amount added to the hitbox size of each successive ball.
	float size_step{1};
	// The initial velocity of spawned balls.
	float initial_velocity{300};
	// Amount added to the velocity of each successive ball.
	float velocity_step{20};

	bool operator==(const ball_settings&) const = default;
};
template <> struct tr::binary_reader<ball_settings> : tr::default_binary_reader<ball_settings> {};
template <> struct tr::binary_writer<ball_settings> : tr::default_binary_writer<ball_settings> {};

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

// Gamemode information.
struct gamemode {
	// Flag denoting a gamemode as built-in.
	bool builtin{false};
	// The name of the gamemode.
	tr::static_string<12 * 4> name{};
	// The author of the gamemode.
	tr::static_string<20 * 4> author{};
	// The description of the gamemode.
	tr::static_string<40 * 4> description{};
	// The name of the song used for the gamemode.
	tr::static_string<12 * 4> song{"classic"};
	// The player settings.
	player_settings player{};
	// The ball settings.
	ball_settings ball{};

	bool operator==(const gamemode&) const = default;

	// Gets the localized name of the gamemode (differs from the name string for built-in gamemodes).
	std::string_view name_loc() const;
	// Gets the localized description of the gamemode (differs from the description string for built-in gamemodes).
	std::string_view description_loc() const;
	// Gets the localized description of the gamemode, falling back to a "no description" message if empty.
	std::string description_loc_with_fallback() const;

	// Saves the gamemode to a file based on its name.
	void save_to_file() const;
};
template <> struct tr::binary_reader<gamemode> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, gamemode& out);
};
template <> struct tr::binary_writer<gamemode> {
	static void write_to_stream(std::ostream& os, const gamemode& in);
};

// Randomly picks a menu gamemode.
gamemode pick_menu_gamemode();

// Container for a gamemode and its path.
struct gamemode_with_path {
	// The path of the gamemode.
	std::string path;
	// The gamemode.
	gamemode gamemode;

	bool operator==(const gamemode_with_path&) const = default;
};
// Loads all available gamemodes.
std::vector<gamemode_with_path> load_gamemodes();

// Saved draft of an unfinished new gamemode.
inline gamemode g_new_gamemode_draft{};