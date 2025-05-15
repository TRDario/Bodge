#include "../include/score.hpp"
#include "../include/gamemode.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

std::strong_ordering operator<=>(const score& l, const score& r) noexcept
{
	return l.result <=> r.result;
}

void tr::binary_reader<score>::read_from_stream(std::istream& is, score& out)
{
	binary_read(is, out.description);
	binary_read(is, out.timestamp);
	binary_read(is, out.result);
	binary_read(is, out.flags);
}

std::span<const std::byte> tr::binary_reader<score>::read_from_span(std::span<const std::byte> span, score& out)
{
	span = binary_read(span, out.description);
	span = binary_read(span, out.timestamp);
	span = binary_read(span, out.result);
	return binary_read(span, out.flags);
}

void tr::binary_writer<score>::write_to_stream(std::ostream& os, const score& in)
{
	binary_write(os, in.description);
	binary_write(os, in.timestamp);
	binary_write(os, in.result);
	binary_write(os, in.flags);
}

std::span<std::byte> tr::binary_writer<score>::write_to_span(std::span<std::byte> span, const score& in)
{
	span = binary_write(span, in.description);
	span = binary_write(span, in.timestamp);
	span = binary_write(span, in.result);
	return binary_write(span, in.flags);
}

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

void scorefile_t::add_score(const gamemode& gamemode, score&& score)
{
	if (!scores.contains(gamemode)) {
		scores.insert({gamemode, {}});
	}
	scores.at(gamemode).insert(std::move(score));
}

void scorefile_t::load_from_file() noexcept
{
	try {
		const std::filesystem::path path{cli_settings.userdir / "scorefile.dat"};
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		data = tr::binary_read(data, name);
		data = tr::binary_read(data, scores);
		data = tr::binary_read(data, playtime);
		LOG(tr::severity::INFO, "Loaded scores from '{}'.", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load scores from '{}': {}.", (cli_settings.userdir / "scorefile.dat").string(), err.what());
	}
}

void scorefile_t::save_to_file() noexcept
{
	const std::filesystem::path path{cli_settings.userdir / "scorefile.dat"};
	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, name);
		tr::binary_write(buffer, scores);
		tr::binary_write(buffer, playtime);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), rand<std::uint8_t>(rng))};
		tr::binary_write(file, std::span{encrypted});
		LOG(tr::severity::INFO, "Saved scores to '{}'.", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save scores to '{}': {}.", path.string(), err.what());
	}
}