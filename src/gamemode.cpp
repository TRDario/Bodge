#include "../include/gamemode.hpp"
#include "../include/settings.hpp"

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

bool player_settings::autoplay() const noexcept
{
	return starting_lives == -1;
}

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

gamemode::gamemode(const std::filesystem::path& path)
{
	std::ifstream file{tr::open_file_r(path, std::ios::binary)};
	tr::binary_read(tr::decrypt(tr::flush_binary(file)), *this);
}

void tr::binary_reader<gamemode>::read_from_stream(std::istream& is, gamemode& out)
{
	binary_read(is, out.name);
	binary_read(is, out.author);
	binary_read(is, out.description);
	binary_read(is, out.difficulty);
	binary_read(is, out.player);
	binary_read(is, out.ball);
}

std::span<const std::byte> tr::binary_reader<gamemode>::read_from_span(std::span<const std::byte> span, gamemode& out)
{
	span = binary_read(span, out.name);
	span = binary_read(span, out.author);
	span = binary_read(span, out.description);
	span = binary_read(span, out.difficulty);
	span = binary_read(span, out.player);
	return binary_read(span, out.ball);
}

void tr::binary_writer<gamemode>::write_to_stream(std::ostream& os, const gamemode& in)
{
	binary_write(os, in.name);
	binary_write(os, in.author);
	binary_write(os, in.description);
	binary_write(os, in.difficulty);
	binary_write(os, in.player);
	binary_write(os, in.ball);
}

std::span<std::byte> tr::binary_writer<gamemode>::write_to_span(std::span<std::byte> span, const gamemode& in)
{
	span = binary_write(span, in.name);
	span = binary_write(span, in.author);
	span = binary_write(span, in.description);
	span = binary_write(span, in.difficulty);
	span = binary_write(span, in.player);
	return binary_write(span, in.ball);
}

void gamemode::save_to_file() noexcept
{
	try {
		const std::filesystem::path base_path{cli_settings.userdir / "gamemodes" / (name + ".gmd")};
		std::ofstream file;
		if (!exists(base_path)) {
			file = tr::open_file_w(base_path, std::ios::binary);
		}
		else {
			int index{0};
			std::filesystem::path path{cli_settings.userdir / "gamemodes" / std::format("{}({}).gmd", name, index++)};
			while (exists(path)) {
				path = cli_settings.userdir / "gamemodes" / std::format("{}({}).gmd", name, index++);
			}
		}

		std::stringstream bufstream;
		tr::binary_write(bufstream, *this);
		tr::binary_write(file, tr::encrypt(tr::range_bytes(bufstream.view()), rand<std::uint8_t>(rng)));
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save gamemode '{}': {}.", name, err.what());
	}
}

std::size_t std::hash<gamemode>::operator()(const gamemode& gamemode) const noexcept
{
	std::hash<std::string> hasher;
	std::size_t hash{hasher(gamemode.name)};
	hash = hash ^ hasher(gamemode.author) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash = hash ^ hasher(gamemode.description) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

std::vector<gamemode> load_gamemodes() noexcept
{
	std::vector<gamemode> gamemodes;
	try {
		const std::filesystem::path gamemode_dir{cli_settings.userdir / "gamemodes"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{gamemode_dir}) {
			const std::filesystem::path path{file};
			try {
				if (!file.is_regular_file() || path.extension() != ".gmd") {
					continue;
				}
				gamemodes.emplace_back(path);
				LOG(tr::severity::INFO, "Loaded gamemode '{}' from '{}'.", gamemodes.back().name, path.string());
			}
			catch (std::exception& err) {
				LOG(tr::severity::ERROR, "Failed to load gamemode from '{}': {}.", path.string(), err.what());
			}
		}
	}
	catch (std::exception& err) {
		LOG(tr::severity::INFO, "Failed to load gamemodes: {}.", err.what());
	}
	return gamemodes;
}