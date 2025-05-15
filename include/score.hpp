#pragma once
#include "gamemode.hpp"

/////////////////////////////////////////////////////////////// SCORE FLAGS ///////////////////////////////////////////////////////////////

// Packed score flags.
struct score_flags {
	// Flag for a prematurely-exited game.
	std::uint32_t exited_prematurely : 1;
	// Flag for a game with a modified game speed.
	std::uint32_t modified_game_speed : 1;
};

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

// Scoreboard entry.
struct score {
	// The description of the score.
	std::string description;
	// The timestamp of the score.
	tr::time_point timestamp;
	// The actual score value.
	ticks result;
	// Packed score flags.
	score_flags flags;

	friend std::strong_ordering operator<=>(const score& l, const score& r) noexcept;
};

// Score binary reader.
template <> struct tr::binary_reader<score> {
	static void read_from_stream(std::istream& is, score& out);
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score& out);
};

// Score binary writer.
template <> struct tr::binary_writer<score> {
	static void write_to_stream(std::ostream& os, const score& in);
	static std::span<std::byte> write_to_span(std::span<std::byte> span, const score& in);
};

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

// File containing score information.
struct scorefile_t {
	// The name of the player.
	std::string name;
	// Score tables.
	std::unordered_map<gamemode, std::set<score, std::greater<score>>> scores;
	// The total recorded playtime.
	ticks playtime{0};

	// Adds a new score to the scorefile.
	void add_score(const gamemode& gamemode, score&& score);
	// Loads the scorefile from file.
	void load_from_file() noexcept;
	// Saves the scorefile to file.
	void save_to_file() noexcept;
};

// The active scorefile.
inline scorefile_t scorefile{};