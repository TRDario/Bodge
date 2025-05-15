#pragma once
#include "global.hpp"

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

// Gamemode player settings.
struct player_settings {
	// The number of starting lives.
	std::int32_t starting_lives{2};
	// The size of the player hitbox.
	float size{10};
	// The player's movement inertia (range: [0, 1]).
	float inertia{0.1};

	bool operator==(const player_settings&) const noexcept = default;

	// Gets whether the gamemode is autoplay.
	bool autoplay() const noexcept;
};

// Sentinel for an autoplay gamemode.
constexpr player_settings NO_PLAYER{-1};

////////////////////////////////////////////////////////////// BALL SETTINGS //////////////////////////////////////////////////////////////

// Gamemode ball settings.
struct ball_settings {
	// The starting number of balls.
	std::uint8_t starting_count{10};
	// The largest possible number of balls.
	std::uint8_t max_count{20};
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
	// The name of the gamemode.
	std::string name{"Untitled"};
	// The author of the gamemode.
	std::string author{"Unknown"};
	// The description of the gamemode.
	std::string description;
	// The difficulty rating of the gamemode.
	std::uint8_t difficulty{50};
	// Player settings.
	player_settings player{};
	// Ball settings.
	ball_settings ball{};

	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates a default gamemode.
	gamemode() noexcept = default;
	// Creates a menu gamemode.
	constexpr gamemode(const ball_settings& ball) noexcept
		: player{NO_PLAYER}, ball{ball}
	{
	}
	// Loads a gamemode from file.
	gamemode(const std::filesystem::path& path);

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	bool operator==(const gamemode&) const noexcept = default;

	// Saves a gamemode to file.
	void save_to_file() noexcept;
};

// Gamemode binary reader.
template <> struct tr::binary_reader<gamemode> {
	static void read_from_stream(std::istream& is, gamemode& out);
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, gamemode& out);
};

// Gamemode binary writer.
template <> struct tr::binary_writer<gamemode> {
	static void write_to_stream(std::ostream& os, const gamemode& in);
	static std::span<std::byte> write_to_span(std::span<std::byte> span, const gamemode& in);
};

// Hashing function for a gamemode.
template <> struct std::hash<gamemode> {
	std::size_t operator()(const gamemode& gamemode) const noexcept;
};

// Loads all found gamemodes.
std::vector<gamemode> load_gamemodes() noexcept;

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The gamemodes that can appear in the main menu background.
constexpr std::array<gamemode, 5> MENU_GAMEMODES{{
	ball_settings{},
	ball_settings{1, 20, 1_s, 10, 2, 250, 10},
	ball_settings{10, 10, 10_s, 50, 0, 400, 0},
	ball_settings{50, 50, 10_s, 10, 0, 400, 0},
	ball_settings{25, 25, 10_s, 20, 0, 350, 0},
}};