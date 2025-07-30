#pragma once
#include "gamemode.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Scorefile version identifier.
constexpr std::uint8_t SCOREFILE_VERSION{0};

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
	tr::static_string<255> description;
	// The timestamp of the score (seconds since UNIX epoch).
	std::int64_t timestamp;
	// The actual score value.
	ticks result;
	// Packed score flags.
	score_flags flags;

	friend std::strong_ordering operator<=>(const score& l, const score& r);
};

// Score binary reader.
template <> struct tr::binary_reader<score> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score& out);
};

// Score binary writer.
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

// Score category binary reader.
template <> struct tr::binary_reader<score_category> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_category& out);
};

// Score category binary writer.
template <> struct tr::binary_writer<score_category> {
	static void write_to_stream(std::ostream& os, const score_category& in);
};

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

// File containing score information.
struct scorefile_t {
	// The name of the player.
	tr::static_string<20> name;
	// Score tables.
	std::vector<score_category> categories;
	// The total recorded playtime.
	ticks playtime{0};
	// The last selected gamemode.
	gamemode last_selected_gamemode;

	// Finds the personal best result for a given gamemode.
	ticks category_pb(const gamemode& gamemode) const;
	// Updates the personal best if the current result is better.
	void update_category(const gamemode& gamemode, ticks pb);

	// Adds a new score to the scorefile.
	void add_score(const gamemode& gamemode, const score& score);
	// Loads the scorefile from file.
	void load_from_file();
	// Saves the scorefile to file.
	void save_to_file();
};

// The active scorefile.
inline scorefile_t scorefile{};