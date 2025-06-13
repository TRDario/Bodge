#include "../include/gamemode.hpp"
#include "../include/settings.hpp"

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

bool player_settings::autoplay() const noexcept
{
	return starting_lives == -1U;
}

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

gamemode::gamemode(const path& path)
{
	ifstream file{open_file_r(path, std::ios::binary)};
	binary_read(decrypt(flush_binary(file)), *this);
}

void tr::binary_reader<gamemode>::read_from_stream(istream& is, gamemode& out)
{
	binary_read(is, out.builtin);
	binary_read(is, out.name);
	binary_read(is, out.author);
	binary_read(is, out.description);
	binary_read(is, out.player);
	binary_read(is, out.ball);
}

span<const byte> tr::binary_reader<gamemode>::read_from_span(span<const byte> span, gamemode& out)
{
	span = binary_read(span, out.builtin);
	span = binary_read(span, out.name);
	span = binary_read(span, out.author);
	span = binary_read(span, out.description);
	span = binary_read(span, out.player);
	return binary_read(span, out.ball);
}

void tr::binary_writer<gamemode>::write_to_stream(ostream& os, const gamemode& in)
{
	binary_write(os, in.builtin);
	binary_write(os, in.name);
	binary_write(os, in.author);
	binary_write(os, in.description);
	binary_write(os, in.player);
	binary_write(os, in.ball);
}

span<byte> tr::binary_writer<gamemode>::write_to_span(span<byte> span, const gamemode& in)
{
	span = binary_write(span, in.builtin);
	span = binary_write(span, in.name);
	span = binary_write(span, in.author);
	span = binary_write(span, in.description);
	span = binary_write(span, in.player);
	return binary_write(span, in.ball);
}

void gamemode::save_to_file() noexcept
{
	try {
		path path{cli_settings.userdir / "gamemodes" / format("{}.gmd", name)};
		ofstream file;
		if (!exists(path)) {
			file = open_file_w(path, std::ios::binary);
		}
		else {
			int index{0};
			do {
				path = cli_settings.userdir / "gamemodes" / format("{}({}).gmd", name, index++);
			} while (exists(path));
			file = open_file_w(path, std::ios::binary);
		}

		std::ostringstream bufstream{std::ios::binary};
		binary_write(bufstream, *this);
		binary_write(file, encrypt(range_bytes(bufstream.view()), rand<u8>(rng)));
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to save gamemode '{}': {}.", name, err.what());
	}
}

vector<gamemode> load_gamemodes() noexcept
{
	vector<gamemode> gamemodes;
	try {
		gamemodes.append_range(BUILTIN_GAMEMODES);
		const path gamemode_dir{cli_settings.userdir / "gamemodes"};
		for (directory_entry file : directory_iterator{gamemode_dir}) {
			const path path{file};
			try {
				if (!file.is_regular_file() || path.extension() != ".gmd") {
					continue;
				}
				gamemodes.emplace_back(path);
				LOG(INFO, "Loaded gamemode '{}' from '{}'.", gamemodes.back().name, path.string());
			}
			catch (std::exception& err) {
				LOG(ERROR, "Failed to load gamemode from '{}': {}.", path.string(), err.what());
			}
		}
	}
	catch (std::exception& err) {
		LOG(INFO, "Failed to load gamemodes: {}.", err.what());
	}
	return gamemodes;
}