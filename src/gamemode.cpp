#include "../include/gamemode.hpp"
#include "../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The gamemodes that can appear in the main menu background.
const std::array<gamemode, 3> BUILTIN_GAMEMODES{
	gamemode{true, "gm_classic", "TRDario", "gm_classic_d", "classic", player_settings{}, ball_settings{1, 50, 10_s, 25, 0, 450, 25}},
	gamemode{true, "gm_chonk", "TRDario", "gm_chonk_d", "chonk", player_settings{}, ball_settings{5, 20, 15_s, 75, 0, 350, 25}},
	gamemode{true, "gm_swarm", "TRDario", "gm_swarm_d", "swarm", player_settings{2, 20, 0.05f},
			 ball_settings{15, 50, 2.5_s, 10, 0, 250, 10}},
};
// The gamemodes that can appear in the main menu background.
constexpr std::array<gamemode, 2> MENU_GAMEMODES{
	gamemode{false, "gm_chonk", "TRDario", "gm_chonk_d", {}, NO_PLAYER, ball_settings{5, 20, 15_s, 75, 0, 350, 25}},
	gamemode{false, "gm_swarm", "TRDario", "gm_swarm_d", {}, NO_PLAYER, ball_settings{15, 50, 2.5_s, 10, 0, 250, 10}},
};

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

bool autoplay(const player_settings& ps)
{
	return ps.starting_lives == std::numeric_limits<std::uint32_t>::max();
}

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

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

gamemode pick_menu_gamemode()
{
	return MENU_GAMEMODES[engine::rng.generate(MENU_GAMEMODES.size())];
}

gamemode load_gamemode(const std::filesystem::path& path)
{
	gamemode gm;
	std::ifstream file{tr::open_file_r(path, std::ios::binary)};
	tr::binary_read(tr::decrypt(tr::flush_binary(file)), gm);
	return gm;
}

std::vector<gamemode> load_gamemodes()
{
	std::vector<gamemode> gamemodes;
	try {
		gamemodes.insert(gamemodes.end(), BUILTIN_GAMEMODES.begin(), BUILTIN_GAMEMODES.end());
		const std::filesystem::path gamemode_dir{engine::cli_settings.userdir / "gamemodes"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{gamemode_dir}) {
			const std::filesystem::path path{file};
			try {
				if (!file.is_regular_file() || path.extension() != ".gmd") {
					continue;
				}
				gamemodes.push_back(load_gamemode(path));
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

void save_gamemode(const gamemode& gm)
{
	std::filesystem::path path{engine::cli_settings.userdir / "gamemodes" / std::format("{}.gmd", gm.name)};
	if (std::filesystem::exists(path)) {
		int index{0};
		do {
			path = engine::cli_settings.userdir / "gamemodes" / std::format("{}({}).gmd", gm.name, index++);
		} while (std::filesystem::exists(path));
	}

	try {
		std::ofstream file = tr::open_file_w(path, std::ios::binary);
		std::ostringstream bufstream{std::ios::binary};
		tr::binary_write(bufstream, gm);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(bufstream.view()), engine::rng.generate<std::uint8_t>())};
		tr::binary_write(file, std::span{encrypted});

		LOG(tr::severity::INFO, "Saved gamemode '{}'.", gm.name);
		LOG_CONTINUE("To: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save gamemode '{}'.", gm.name);
		LOG_CONTINUE("To: '{}'", path.string());
		LOG_CONTINUE(err);
	}
}

std::string_view name(const gamemode& gm)
{
	return gm.builtin ? engine::loc[gm.name] : std::string_view{gm.name};
}

std::string_view description(const gamemode& gm)
{
	return gm.builtin ? engine::loc[gm.description] : std::string_view{gm.description};
}