#include "../include/gamemode.hpp"
#include "../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

// Gamemode file version identifier.
constexpr u8 GAMEMODE_VERSION{1};

// Built-in gamemodes that are always available.
const std::array<gamemode, 4> BUILTIN_GAMEMODES{{
	{
		.builtin = true,
		.name = "gm_classic",
		.author = "TRDario",
		.description = "gm_classic_d",
		.song = "classic",
		.player{.life_fragment_spawn_interval = 41.67_s},
		.ball{.starting_count = 1,
			  .max_count = 50,
			  .spawn_interval = 8.334_s,
			  .initial_size = 20,
			  .size_step = 0,
			  .initial_velocity = 300,
			  .velocity_step = 10},
	},
	{
		.builtin = true,
		.name = "gm_chonk",
		.author = "TRDario",
		.description = "gm_chonk_d",
		.song = "chonk",
		.player{.hitbox_radius = 30},
		.ball{.starting_count = 3,
			  .max_count = 10,
			  .spawn_interval = 19.59183674_s,
			  .initial_size = 70,
			  .size_step = 0,
			  .initial_velocity = 350,
			  .velocity_step = 25},
	},
	{
		.builtin = true,
		.name = "gm_swarm",
		.author = "TRDario",
		.description = "gm_swarm_d",
		.song = "swarm",
		.player{.life_fragment_spawn_interval = 27.42857144_s,
				.hitbox_radius = 8,
				.inertia_factor = 0.05f},
		.ball{.starting_count = 10,
			  .max_count = 40,
			  .spawn_interval = 3.42857143_s,
			  .initial_size = 10,
			  .size_step = 0,
			  .initial_velocity = 250,
			  .velocity_step = 10},
	},
	{
		.builtin = true,
		.name = "gm_variety",
		.author = "TRDario",
		.description = "gm_variety_d",
		.song = "variety",
		.player{.starting_lives = 0,
			    .life_fragment_spawn_interval = 15.36_s},
		.ball{.starting_count = 1,
			  .max_count = 25,
			  .spawn_interval = 3.84_s,
			  .initial_size = 10,
			  .size_step = 1.5f,
			  .initial_velocity = 300,
			  .velocity_step = 0},
	},
}};

// clang-format on
//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

std::string_view gamemode::name_loc() const
{
	return builtin ? g_loc[name] : std::string_view{name};
}

std::string_view gamemode::description_loc() const
{
	return builtin ? g_loc[description] : std::string_view{description};
}

std::string gamemode::description_loc_with_fallback() const
{
	std::string_view base{description_loc()};
	return std::string{base.empty() ? g_loc["no_description"] : base};
}

void gamemode::save_to_file() const
{
	std::filesystem::path path{g_cli_settings.user_directory / "gamemodes" / TR_FMT::format("{}.gmd", name)};
	if (std::filesystem::exists(path)) {
		int index{0};
		do {
			path = g_cli_settings.user_directory / "gamemodes" / TR_FMT::format("{}({}).gmd", name, index++);
		} while (std::filesystem::exists(path));
	}

	try {
		std::ofstream file = tr::open_file_w(path, std::ios::binary);
		std::ostringstream bufstream{std::ios::binary};
		tr::binary_write(bufstream, *this);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(bufstream.view()), g_rng.generate<u8>())};
		tr::binary_write(file, GAMEMODE_VERSION);
		tr::binary_write(file, std::span{encrypted});
	}
	catch (std::exception&) {
		return;
	}
}

std::span<const std::byte> tr::binary_reader<gamemode>::read_from_span(std::span<const std::byte> span, gamemode& out)
{
	span = tr::binary_read(span, out.builtin);
	span = tr::binary_read(span, out.name);
	span = tr::binary_read(span, out.author);
	span = tr::binary_read(span, out.description);
	span = tr::binary_read(span, out.song);
	span = tr::binary_read(span, out.player);
	return tr::binary_read(span, out.ball);
}

void tr::binary_writer<gamemode>::write_to_stream(std::ostream& os, const gamemode& in)
{
	tr::binary_write(os, in.builtin);
	tr::binary_write(os, in.name);
	tr::binary_write(os, in.author);
	tr::binary_write(os, in.description);
	tr::binary_write(os, in.song);
	tr::binary_write(os, in.player);
	tr::binary_write(os, in.ball);
}

//

gamemode pick_menu_gamemode()
{
	// clang-format off
	constexpr std::array<gamemode, 6> MENU_GAMEMODES{{
		{.ball{.starting_count = 15, .max_count = 15, .initial_size = 20, .size_step = 0, .initial_velocity = 350,  .velocity_step = 0}},
		{.ball{.starting_count = 12, .max_count = 12, .initial_size = 25, .size_step = 0, .initial_velocity = 400,  .velocity_step = 0}},
		{.ball{.starting_count = 5,  .max_count = 5,  .initial_size = 25, .size_step = 0, .initial_velocity = 1000, .velocity_step = 0}},
		{.ball{.starting_count = 8,  .max_count = 8,  .initial_size = 50, .size_step = 0, .initial_velocity = 350,  .velocity_step = 25}},
		{.ball{.starting_count = 6,  .max_count = 6,  .initial_size = 75, .size_step = 0, .initial_velocity = 350,  .velocity_step = 25}},
		{.ball{.starting_count = 25, .max_count = 25, .initial_size = 10, .size_step = 0, .initial_velocity = 250,  .velocity_step = 10}},
	}};
	// clang-format on

	return MENU_GAMEMODES[g_rng.generate(MENU_GAMEMODES.size())];
}

std::vector<gamemode_with_path> load_gamemodes()
{
	std::vector<gamemode_with_path> gamemodes;
	try {
		for (const gamemode& builtin : BUILTIN_GAMEMODES) {
			gamemodes.emplace_back(std::string{}, builtin);
		}
		const std::filesystem::path gamemode_dir{g_cli_settings.user_directory / "gamemodes"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{gamemode_dir}) {
			const std::filesystem::path path{file};
			try {
				if (!file.is_regular_file() || path.extension() != ".gmd") {
					continue;
				}

				std::ifstream is{tr::open_file_r(path, std::ios::binary)};
				if (tr::binary_read<u8>(is) == GAMEMODE_VERSION) {
					gamemodes.emplace_back(path.string(), tr::binary_read<gamemode>(tr::decrypt(tr::flush_binary(is))));
				}
			}
			catch (std::exception&) {
				continue;
			}
		}
	}
	catch (std::exception&) {
		return gamemodes;
	}
	return gamemodes;
}