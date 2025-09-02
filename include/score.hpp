#pragma once
#include "gamemode.hpp"

/////////////////////////////////////////////////////////////// SCORE FLAGS ///////////////////////////////////////////////////////////////

struct score_flags {
	u32 exited_prematurely : 1;
	u32 modified_game_speed : 1;
};
template <> struct tr::binary_reader<score_flags> : tr::default_binary_reader<score_flags> {};
template <> struct tr::binary_writer<score_flags> : tr::default_binary_writer<score_flags> {};

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

struct score_entry {
	tr::static_string<255 * 4> description;
	i64 unix_timestamp;
	i64 score;
	ticks time;
	score_flags flags;

	friend std::strong_ordering operator<=>(const score_entry& l, const score_entry& r);
};
template <> struct tr::binary_reader<score_entry> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_entry& out);
};
template <> struct tr::binary_writer<score_entry> {
	static void write_to_stream(std::ostream& os, const score_entry& in);
};

///////////////////////////////////////////////////////////// SCORE CATEGORY //////////////////////////////////////////////////////////////

struct score_category {
	gamemode gamemode;
	i64 best_score;
	ticks best_time;
	std::vector<score_entry> scores;
};
template <> struct tr::binary_reader<score_category> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_category& out);
};
template <> struct tr::binary_writer<score_category> {
	static void write_to_stream(std::ostream& os, const score_category& in);
};

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

struct bests {
	i64 score;
	ticks time;
};

struct scorefile {
	tr::static_string<20 * 4> name;
	std::vector<score_category> categories;
	ticks playtime{0};
	gamemode last_selected;

	bests bests(const gamemode& gm) const;
	void update_bests(const gamemode& gm, i64 score, ticks time);
	void add_score(const gamemode& gm, const score_entry& s);
};

///////////////////////////////////////////////////////////////// ENGINE //////////////////////////////////////////////////////////////////

namespace engine {
	inline scorefile scorefile;
	void load_scorefile();
	void save_scorefile();
} // namespace engine