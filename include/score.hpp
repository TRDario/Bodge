///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides score structures and scorefile functionality.                                                                                //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "gamemode.hpp"

/////////////////////////////////////////////////////////////// SCORE FLAGS ///////////////////////////////////////////////////////////////

// Score flags.
struct score_flags {
	// Denotes that the game exited prematurely.
	u32 exited_prematurely : 1;
	// Denotes that a modified game speed was detected.
	u32 modified_game_speed : 1;
};
template <> struct tr::binary_reader<score_flags> : tr::default_binary_reader<score_flags> {};
template <> struct tr::binary_writer<score_flags> : tr::default_binary_writer<score_flags> {};

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

// Score entry information.
struct score_entry {
	// Description provided by the player for the score.
	tr::static_string<255 * 4> description;
	// The UNIX timestamp of when the game happened.
	i64 timestamp;
	// The achieved score.
	i64 score;
	// The achieved time.
	ticks time;
	// Additional score flags.
	score_flags flags;
};
template <> struct tr::binary_reader<score_entry> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_entry& out);
};
template <> struct tr::binary_writer<score_entry> {
	static void write_to_stream(std::ostream& os, const score_entry& in);
};

// Compares score entries by score.
bool compare_scores(const score_entry& l, const score_entry& r);
// Compares score entries by time.
bool compare_times(const score_entry& l, const score_entry& r);

///////////////////////////////////////////////////////////// SCORE CATEGORY //////////////////////////////////////////////////////////////

// Collection of scores played on a specific gamemode.
struct score_category {
	// The gamemode the scores were played on.
	gamemode gamemode;
	// Cached best score result.
	i64 best_score;
	// Cached best time result.
	ticks best_time;
	// List of score entries.
	std::vector<score_entry> entries;
};
template <> struct tr::binary_reader<score_category> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_category& out);
};
template <> struct tr::binary_writer<score_category> {
	static void write_to_stream(std::ostream& os, const score_category& in);
};

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

// Structure returned by scorefile::bests() containing the best results for a given category.
struct bests {
	// The best score result.
	i64 score;
	// The best time result.
	ticks time;
};

// Player scorefile.
inline struct scorefile {
	// The name of the player the scorefile belongs to.
	tr::static_string<20 * 4> name;
	// List of score categories.
	std::vector<score_category> categories;
	// Total playtime.
	ticks playtime{0};
	// Copy of the last gamemode selected in the "start game" menu.
	gamemode last_selected;

	// Loads the scorefile.
	void load_from_file();
	// Saves the scorefile.
	void save_to_file() const;

	// Gets the best results for a specific gamemode.
	bests bests(const gamemode& gm) const;
	// Formats the player info into a string to be displayed.
	std::string format_player_info() const;

	// Adds a score entry to the scorefile.
	void add_score(const gamemode& gm, const score_entry& s);
} g_scorefile{}; // Global copy of the scorefile.