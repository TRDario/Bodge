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
	scores.at(gamemode).insert(std::upper_bound(scores.at(gamemode).begin(), scores.at(gamemode).end(), score), std::move(score));
}

void scorefile_t::load_from_file() noexcept
{
	try {
		const path path{cli_settings.userdir / "scorefile.dat"};
		ifstream file{open_file_r(path, std::ios::binary)};
		vector<std::byte> raw{decrypt(flush_binary(file))};
		std::span<const std::byte> data{raw};
		data = binary_read(data, name);
		data = binary_read(data, scores);
		data = binary_read(data, playtime);
		LOG(INFO, "Loaded scores from '{}'.", path.string());
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to load scores from '{}': {}.", (cli_settings.userdir / "scorefile.dat").string(), err.what());
	}
}

void scorefile_t::save_to_file() noexcept
{
	const path path{cli_settings.userdir / "scorefile.dat"};
	try {
		ofstream file{open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		binary_write(buffer, name);
		binary_write(buffer, scores);
		binary_write(buffer, playtime);
		const vector<std::byte> encrypted{encrypt(range_bytes(buffer.view()), rand<u8>(rng))};
		binary_write(file, std::span{encrypted});
		LOG(INFO, "Saved scores to '{}'.", path.string());
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to save scores to '{}': {}.", path.string(), err.what());
	}
}