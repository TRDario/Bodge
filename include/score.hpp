#pragma once
#include "gamemode.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Scorefile version identifier.
constexpr std::uint8_t SCOREFILE_VERSION{1};

/////////////////////////////////////////////////////////////// SCORE FLAGS ///////////////////////////////////////////////////////////////

// Packed score flags.
struct score_flags {
	// Flag for a prematurely-exited game.
	std::uint32_t exited_prematurely : 1;
	// Flag for a game with a modified game speed.
	std::uint32_t modified_game_speed : 1;
};
template <> struct tr::binary_reader<score_flags> : tr::default_binary_reader<score_flags> {};
template <> struct tr::binary_writer<score_flags> : tr::default_binary_writer<score_flags> {};

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

// Scoreboard entry.
struct score {
	// The description of the score.
	tr::static_string<255 * 4> description;
	// The timestamp of the score (seconds since UNIX epoch).
	std::int64_t timestamp;
	// The actual score value.
	ticks result;
	// Packed score flags.
	score_flags flags;

	friend std::strong_ordering operator<=>(const score& l, const score& r);
};
template <> struct tr::binary_reader<score> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score& out);
};
template <> struct tr::binary_writer<score> {
	static void write_to_stream(std::ostream& os, const score& in);
};

///////////////////////////////////////////////////////////// SCORE CATEGORY //////////////////////////////////////////////////////////////

// A score category.
struct score_category {
	// The gamemode defining the category.
	gamemode gamemode;
	// The personal best time for this category.
	ticks pb;
	// The scores in the category.
	std::vector<score> scores;
};
template <> struct tr::binary_reader<score_category> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_category& out);
};
template <> struct tr::binary_writer<score_category> {
	static void write_to_stream(std::ostream& os, const score_category& in);
};

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

// File containing score information.
struct scorefile {
	// The name of the player.
	tr::static_string<20 * 4> name;
	// Score tables.
	std::vector<score_category> categories;
	// The total recorded playtime.
	ticks playtime{0};
	// The last selected gamemode.
	gamemode last_selected;
};
// Finds the personal best result for a given gamemode.
ticks pb(const scorefile& sf, const gamemode& gm);
// Updates the personal best for a given gamemode.
void update_pb(scorefile& sf, const gamemode& gm, ticks pb);
// Adds a new score to the scorefile.
void add_score(scorefile& sf, const gamemode& gm, const score& s);

///////////////////////////////////////////////////////////////// ENGINE //////////////////////////////////////////////////////////////////

namespace engine {
	// The loaded scorefile.
	inline scorefile scorefile;
	// Loads the scorefile from file.
	void load_scorefile();
	// Saves the scorefile to file.
	void save_scorefile();
} // namespace engine