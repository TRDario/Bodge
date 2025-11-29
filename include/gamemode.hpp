#pragma once
#include "global.hpp"

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

struct player_settings {
	u8 starting_lives{2};
	bool spawn_life_fragments{true};
	ticks life_fragment_spawn_interval{45_s};
	float hitbox_radius{10};
	float inertia_factor{0.1f};

	bool operator==(const player_settings&) const = default;
};
template <> struct tr::binary_reader<player_settings> : tr::default_binary_reader<player_settings> {};
template <> struct tr::binary_writer<player_settings> : tr::default_binary_writer<player_settings> {};

////////////////////////////////////////////////////////////// BALL SETTINGS //////////////////////////////////////////////////////////////

struct ball_settings {
	u8 starting_count{10};
	u8 max_count{20};
	ticks spawn_interval{10_s};
	float initial_size{20};
	float size_step{1};
	float initial_velocity{300};
	float velocity_step{20};

	bool operator==(const ball_settings&) const = default;
};
template <> struct tr::binary_reader<ball_settings> : tr::default_binary_reader<ball_settings> {};
template <> struct tr::binary_writer<ball_settings> : tr::default_binary_writer<ball_settings> {};

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

struct gamemode {
	bool builtin{false};
	tr::static_string<12 * 4> name{};
	tr::static_string<20 * 4> author{};
	tr::static_string<40 * 4> description{};
	tr::static_string<12 * 4> song{};
	player_settings player{};
	ball_settings ball{};

	bool operator==(const gamemode&) const = default;

	std::string_view name_loc() const;
	std::string_view description_loc() const;

	void save_to_file() const;
};
template <> struct tr::binary_reader<gamemode> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, gamemode& out);
};
template <> struct tr::binary_writer<gamemode> {
	static void write_to_stream(std::ostream& os, const gamemode& in);
};

gamemode pick_menu_gamemode();
std::vector<gamemode> load_gamemodes();