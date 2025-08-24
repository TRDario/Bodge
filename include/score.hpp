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

struct score {
	tr::static_string<255 * 4> description;
	i64 unix_timestamp;
	ticks result;
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

struct score_category {
	gamemode gamemode;
	ticks personal_best;
	std::vector<score> scores;
};
template <> struct tr::binary_reader<score_category> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_category& out);
};
template <> struct tr::binary_writer<score_category> {
	static void write_to_stream(std::ostream& os, const score_category& in);
};

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

struct scorefile {
	tr::static_string<20 * 4> name;
	std::vector<score_category> categories;
	ticks playtime{0};
	gamemode last_selected;

	ticks personal_best(const gamemode& gm);
	void update_personal_best(const gamemode& gm, ticks pb);
	void add_score(const gamemode& gm, const score& s);
};

///////////////////////////////////////////////////////////////// ENGINE //////////////////////////////////////////////////////////////////

namespace engine {
	inline scorefile scorefile;
	void load_scorefile();
	void save_scorefile();
} // namespace engine