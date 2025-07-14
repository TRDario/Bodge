#include "../include/gamemode.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"

///////////////////////////////////////////////////////////// PLAYER SETTINGS /////////////////////////////////////////////////////////////

bool player_settings::autoplay() const noexcept
{
	return starting_lives == std::numeric_limits<std::uint32_t>::max();
}

//////////////////////////////////////////////////////////////// GAMEMODE /////////////////////////////////////////////////////////////////

gamemode::gamemode() noexcept
	: author{scorefile.name}
{
}

gamemode::gamemode(const std::filesystem::path& path)
{
	std::ifstream file{tr::open_file_r(path, std::ios::binary)};
	tr::binary_read(tr::decrypt(tr::flush_binary(file)), *this);
}

void tr::binary_reader<gamemode>::read_from_stream(std::istream& is, gamemode& out)
{
	tr::binary_read(is, out.builtin);
	tr::binary_read(is, out.name);
	tr::binary_read(is, out.author);
	tr::binary_read(is, out.description);
	tr::binary_read(is, out.player);
	tr::binary_read(is, out.ball);
}

std::span<const std::byte> tr::binary_reader<gamemode>::read_from_span(std::span<const std::byte> span, gamemode& out)
{
	span = tr::binary_read(span, out.builtin);
	span = tr::binary_read(span, out.name);
	span = tr::binary_read(span, out.author);
	span = tr::binary_read(span, out.description);
	span = tr::binary_read(span, out.player);
	return tr::binary_read(span, out.ball);
}

void tr::binary_writer<gamemode>::write_to_stream(std::ostream& os, const gamemode& in)
{
	tr::binary_write(os, in.builtin);
	tr::binary_write(os, in.name);
	tr::binary_write(os, in.author);
	tr::binary_write(os, in.description);
	tr::binary_write(os, in.player);
	tr::binary_write(os, in.ball);
}

std::span<std::byte> tr::binary_writer<gamemode>::write_to_span(std::span<std::byte> span, const gamemode& in)
{
	span = tr::binary_write(span, in.builtin);
	span = tr::binary_write(span, in.name);
	span = tr::binary_write(span, in.author);
	span = tr::binary_write(span, in.description);
	span = tr::binary_write(span, in.player);
	return tr::binary_write(span, in.ball);
}

std::string_view gamemode::name_loc() const noexcept
{
	return builtin ? localization[name] : std::string_view{name};
}

std::string_view gamemode::description_loc() const noexcept
{
	return builtin ? localization[description] : description.empty() ? localization["no_description"] : std::string_view{description};
}

void gamemode::save_to_file() noexcept
{
	try {
		std::filesystem::path path{cli_settings.userdir / "gamemodes" / std::format("{}.gmd", name)};
		std::ofstream file;
		if (!std::filesystem::exists(path)) {
			file = tr::open_file_w(path, std::ios::binary);
		}
		else {
			int index{0};
			do {
				path = cli_settings.userdir / "gamemodes" / std::format("{}({}).gmd", name, index++);
			} while (std::filesystem::exists(path));
			file = tr::open_file_w(path, std::ios::binary);
		}

		std::ostringstream bufstream{std::ios::binary};
		tr::binary_write(bufstream, *this);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(bufstream.view()), tr::rand<std::uint8_t>(rng))};
		tr::binary_write(file, std::span{encrypted});
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save gamemode:");
		LOG_CONTINUE("{}", err.what());
	}
}

std::vector<gamemode> load_gamemodes() noexcept
{
	std::vector<gamemode> gamemodes;
	try {
		gamemodes.insert(gamemodes.end(), BUILTIN_GAMEMODES.begin(), BUILTIN_GAMEMODES.end());
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
				LOG(tr::severity::ERROR, "Failed to load gamemode from '{}':", path.string());
				LOG_CONTINUE("{}", err.what());
			}
		}
	}
	catch (std::exception& err) {
		LOG(tr::severity::INFO, "Failed to load gamemodes:");
		LOG_CONTINUE("{}", err.what());
	}
	return gamemodes;
}