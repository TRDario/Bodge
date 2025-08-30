#include "../include/gamemode.hpp"
#include "../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Gamemode file version identifier.
constexpr u8 GAMEMODE_VERSION{1};

// The gamemodes that can appear in the main menu background.
const std::array<gamemode, 3> BUILTIN_GAMEMODES{
	gamemode{true, "gm_classic", "TRDario", "gm_classic_d", "classic", player_settings{.life_fragment_spawn_interval = 41.67_s},
			 ball_settings{4, 50, 8.334_s, 20, 0, 375, 10}},
	gamemode{true, "gm_chonk", "TRDario", "gm_chonk_d", "chonk", player_settings{.hitbox_radius = 30},
			 ball_settings{3, 10, 19.59183674_s, 70, 0, 350, 25}},
	gamemode{true, "gm_swarm", "TRDario", "gm_swarm_d", "swarm",
			 player_settings{.life_fragment_spawn_interval = 27.42857144_s, .hitbox_radius = 8, .inertia_factor = 0.05f},
			 ball_settings{10, 40, 3.42857143_s, 10, 0, 250, 10}},
};
// The gamemodes that can appear in the main menu background.
constexpr std::array<gamemode, 4> MENU_GAMEMODES{
	gamemode{.ball{12, 12, 10_s, 25, 0, 350, 0}},
	gamemode{.ball{5, 5, 10_s, 25, 0, 1000, 0}},
	gamemode{.ball{5, 15, 19.59183674_s, 75, 0, 350, 25}},
	gamemode{.ball{15, 30, 3.42857143_s, 10, 0, 250, 10}},
};

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

std::string_view gamemode::name_loc() const
{
	return builtin ? engine::loc[name] : std::string_view{name};
}

std::string_view gamemode::description_loc() const
{
	return builtin ? engine::loc[description] : std::string_view{description};
}

void gamemode::save_to_file() const
{
	std::filesystem::path path{engine::cli_settings.user_directory / "gamemodes" / TR_FMT::format("{}.gmd", name)};
	if (std::filesystem::exists(path)) {
		int index{0};
		do {
			path = engine::cli_settings.user_directory / "gamemodes" / TR_FMT::format("{}({}).gmd", name, index++);
		} while (std::filesystem::exists(path));
	}

	try {
		std::ofstream file = tr::open_file_w(path, std::ios::binary);
		std::ostringstream bufstream{std::ios::binary};
		tr::binary_write(bufstream, *this);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(bufstream.view()), engine::rng.generate<u8>())};
		tr::binary_write(file, GAMEMODE_VERSION);
		tr::binary_write(file, std::span{encrypted});

		LOG(tr::severity::INFO, "Saved gamemode '{}'.", name);
		LOG_CONTINUE("To: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save gamemode '{}'.", name);
		LOG_CONTINUE("To: '{}'", path.string());
		LOG_CONTINUE(err);
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

gamemode engine::pick_menu_gamemode()
{
	return MENU_GAMEMODES[engine::rng.generate(MENU_GAMEMODES.size())];
}

std::vector<gamemode> engine::load_gamemodes()
{
	std::vector<gamemode> gamemodes;
	try {
		gamemodes.insert(gamemodes.end(), BUILTIN_GAMEMODES.begin(), BUILTIN_GAMEMODES.end());
		const std::filesystem::path gamemode_dir{engine::cli_settings.user_directory / "gamemodes"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{gamemode_dir}) {
			const std::filesystem::path path{file};
			try {
				if (!file.is_regular_file() || path.extension() != ".gmd") {
					continue;
				}
				std::ifstream is{tr::open_file_r(path, std::ios::binary)};
				if (tr::binary_read<u8>(is) != GAMEMODE_VERSION) {
					LOG(tr::severity::WARN, "Ignored corrupt or legacy gamemode file.");
					LOG_CONTINUE("From: '{}'", path.string());
					continue;
				}
				gamemodes.push_back(tr::binary_read<gamemode>(tr::decrypt(tr::flush_binary(is))));
				LOG(tr::severity::INFO, "Loaded gamemode '{}'.", gamemodes.back().name);
				LOG_CONTINUE("From: '{}'", path.string());
			}
			catch (std::exception& err) {
				LOG(tr::severity::ERROR, "Failed to load gamemode.");
				LOG_CONTINUE("From: '{}'", path.string());
				LOG_CONTINUE(err);
			}
		}
	}
	catch (std::exception& err) {
		LOG(tr::severity::INFO, "Failed to load gamemodes.");
		LOG_CONTINUE(err);
	}
	return gamemodes;
}