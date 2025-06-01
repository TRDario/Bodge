#pragma once
#include "global.hpp"

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

// Gamemode player settings.
struct player_settings {
	// The number of starting lives.
	u32 starting_lives{2};
	// The size of the player hitbox.
	float size{10};
	// The player's movement inertia (range: [0, 1]).
	float inertia{0.1};

	bool operator==(const player_settings&) const noexcept = default;

	// Gets whether the gamemode is autoplay.
	bool autoplay() const noexcept;
};

// Sentinel for an autoplay gamemode.
constexpr player_settings NO_PLAYER{-1U};

////////////////////////////////////////////////////////////// BALL SETTINGS //////////////////////////////////////////////////////////////

// Gamemode ball settings.
struct ball_settings {
	// The starting number of balls.
	u8 starting_count{10};
	// The largest possible number of balls.
	u8 max_count{20};
	// The interval between ball spawns in ticks.
	ticks spawn_interval{10_s};
	// The starting size of ball spawns.
	float starting_size{20};
	// The amount added to the new ball size on every spawn.
	float size_step{1};
	// The starting velocity of ball spawns.
	float starting_velocity{300};
	// The amount added to the new ball velocity on every spawn.
	float velocity_step{20};

	bool operator==(const ball_settings&) const noexcept = default;
};

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

// Gamemode object.
struct gamemode {
	// Flag marking the gamemode as a special, built-in gamemode.
	bool builtin{false};
	// The name of the gamemode.
	static_string<12> name{"Untitled"};
	// The author of the gamemode.
	static_string<20> author{"Unknown"};
	// The description of the gamemode.
	static_string<40> description;
	// Player settings.
	player_settings player{};
	// Ball settings.
	ball_settings ball{};

	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a default gamemode.
	gamemode() noexcept = default;
	// Creates a builtin gamemode.
	constexpr gamemode(string_view name, string_view description, const player_settings& player, const ball_settings& ball) noexcept
		: builtin{true}, name{name}, author{"TRDario"}, description{description}, player{player}, ball{ball}
	{
	}
	// Creates a menu gamemode.
	constexpr gamemode(const ball_settings& ball) noexcept
		: author{"TRDario"}, player{NO_PLAYER}, ball{ball}
	{
	}
	// Loads a gamemode from file.
	gamemode(const path& path);

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	bool operator==(const gamemode&) const noexcept = default;

	// Saves a gamemode to file.
	void save_to_file() noexcept;
};

// Gamemode binary reader.
template <> struct tr::binary_reader<gamemode> {
	static void read_from_stream(istream& is, gamemode& out);
	static span<const byte> read_from_span(span<const byte> span, gamemode& out);
};

// Gamemode binary writer.
template <> struct tr::binary_writer<gamemode> {
	static void write_to_stream(ostream& os, const gamemode& in);
	static span<byte> write_to_span(span<byte> span, const gamemode& in);
};

// Loads all found gamemodes.
vector<gamemode> load_gamemodes() noexcept;

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The gamemodes that can appear in the main menu background.
const array<gamemode, 2> BUILTIN_GAMEMODES{
	gamemode{"gm_test", "gm_test_d", player_settings{}, ball_settings{10, 10, 10_s, 50, 0, 400, 0}},
	gamemode{"gm_test2", "gm_test2_d", player_settings{}, ball_settings{10, 10, 10_s, 50, 0, 400, 0}},
};

// The gamemodes that can appear in the main menu background.
constexpr array<gamemode, 5> MENU_GAMEMODES{
	ball_settings{},
	ball_settings{1, 20, 1_s, 10, 2, 250, 10},
	ball_settings{10, 10, 10_s, 50, 0, 400, 0},
	ball_settings{50, 50, 10_s, 10, 0, 400, 0},
	ball_settings{25, 25, 10_s, 20, 0, 350, 0},
};