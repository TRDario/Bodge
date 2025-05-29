#pragma once
#include "gamemode.hpp"

/////////////////////////////////////////////////////////////// SCORE FLAGS ///////////////////////////////////////////////////////////////

// Packed score flags.
struct score_flags {
	// Flag for a prematurely-exited game.
	u32 exited_prematurely : 1;
	// Flag for a game with a modified game speed.
	u32 modified_game_speed : 1;
};

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

// Scoreboard entry.
struct score {
	// The description of the score.
	string description;
	// The timestamp of the score (seconds since UNIX epoch).
	std::int64_t timestamp;
	// The actual score value.
	ticks result;
	// Packed score flags.
	score_flags flags;

	friend std::strong_ordering operator<=>(const score& l, const score& r) noexcept;
};

// Score binary reader.
template <> struct tr::binary_reader<score> {
	static void read_from_stream(istream& is, score& out);
	static span<const byte> read_from_span(span<const byte> span, score& out);
};

// Score binary writer.
template <> struct tr::binary_writer<score> {
	static void write_to_stream(ostream& os, const score& in);
	static span<byte> write_to_span(span<byte> span, const score& in);
};

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

// File containing score information.
struct scorefile_t {
	// The name of the player.
	string name;
	// Score tables.
	unordered_map<gamemode, vector<score>> scores;
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