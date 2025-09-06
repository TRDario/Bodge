#include "score.hpp"
#include "settings.hpp"

struct player_settings_v0 {
	u32 starting_lives;
	float hitbox_radius;
	float inertia_factor;
};
template <> struct tr::binary_reader<player_settings_v0> : tr::default_binary_reader<player_settings_v0> {};

struct ball_settings_v0 {
	u8 starting_count;
	u8 max_count;
	ticks spawn_interval;
	float initial_size;
	float size_step;
	float initial_velocity;
	float velocity_step;
};
template <> struct tr::binary_reader<ball_settings_v0> : tr::default_binary_reader<ball_settings_v0> {};

struct gamemode_v0 {
	bool builtin{false};
	tr::static_string<12 * 4> name{};
	tr::static_string<20 * 4> author{};
	tr::static_string<40 * 4> description{};
	tr::static_string<12 * 4> song{};
	player_settings_v0 player{};
	ball_settings_v0 ball{};
};
template <> struct tr::binary_reader<gamemode_v0> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, gamemode_v0& out);
};

struct score_v0 {
	tr::static_string<255 * 4> description;
	i64 unix_timestamp;
	ticks result;
	score_flags flags;

	friend std::strong_ordering operator<=>(const score_entry& l, const score_entry& r);
};
template <> struct tr::binary_reader<score_v0> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_v0& out);
};

struct score_category_v0 {
	gamemode_v0 gamemode;
	ticks personal_best;
	std::vector<score_v0> scores;
};
template <> struct tr::binary_reader<score_category_v0> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, score_category_v0& out);
};

struct settings_v0 {
	u16 window_size;
	display_mode display_mode;
	u8 msaa;
	u16 primary_hue;
	u16 secondary_hue;
	u8 sfx_volume;
	u8 music_volume;
	language_code language;
};
template <> struct tr::binary_reader<settings_v0> : tr::default_binary_reader<settings_v0> {};