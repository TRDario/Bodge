#include "../include/gamemode.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

std::strong_ordering operator<=>(const score& l, const score& r) noexcept
{
	return l.result <=> r.result;
}

void tr::binary_reader<score>::read_from_stream(std::istream& is, score& out)
{
	tr::binary_read(is, out.description);
	tr::binary_read(is, out.timestamp);
	tr::binary_read(is, out.result);
	tr::binary_read(is, out.flags);
}

std::span<const std::byte> tr::binary_reader<score>::read_from_span(std::span<const std::byte> span, score& out)
{
	span = tr::binary_read(span, out.description);
	span = tr::binary_read(span, out.timestamp);
	span = tr::binary_read(span, out.result);
	return tr::binary_read(span, out.flags);
}

void tr::binary_writer<score>::write_to_stream(std::ostream& os, const score& in)
{
	tr::binary_write(os, in.description);
	tr::binary_write(os, in.timestamp);
	tr::binary_write(os, in.result);
	tr::binary_write(os, in.flags);
}

std::span<std::byte> tr::binary_writer<score>::write_to_span(std::span<std::byte> span, const score& in)
{
	span = tr::binary_write(span, in.description);
	span = tr::binary_write(span, in.timestamp);
	span = tr::binary_write(span, in.result);
	return tr::binary_write(span, in.flags);
}

///////////////////////////////////////////////////////////// SCORE CATEGORY //////////////////////////////////////////////////////////////

void tr::binary_reader<score_category>::read_from_stream(std::istream& is, score_category& out)
{
	tr::binary_read(is, out.gamemode);
	tr::binary_read(is, out.scores);
}

std::span<const std::byte> tr::binary_reader<score_category>::read_from_span(std::span<const std::byte> span, score_category& out)
{
	span = tr::binary_read(span, out.gamemode);
	return tr::binary_read(span, out.scores);
}

void tr::binary_writer<score_category>::write_to_stream(std::ostream& os, const score_category& in)
{
	tr::binary_write(os, in.gamemode);
	tr::binary_write(os, in.scores);
}

std::span<std::byte> tr::binary_writer<score_category>::write_to_span(std::span<std::byte> span, const score_category& in)
{
	span = tr::binary_write(span, in.gamemode);
	return tr::binary_write(span, in.scores);
}

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

ticks scorefile_t::pb(const gamemode& gamemode) const noexcept
{
	std::vector<score_category>::const_iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gamemode; })};
	return it == categories.end() ? 0 : it->scores.front().result;
}

void scorefile_t::add_score(const gamemode& gamemode, const score& score)
{
	std::vector<score_category>::iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gamemode; })};
	if (it == categories.end()) {
		it = categories.insert(it, {gamemode, {}});
	}
	it->scores.insert(std::upper_bound(it->scores.begin(), it->scores.end(), score, std::greater<>{}), score);
}

void scorefile_t::load_from_file() noexcept
{
	try {
		const std::filesystem::path path{cli_settings.userdir / "scorefile.dat"};
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		data = tr::binary_read(data, name);
		data = tr::binary_read(data, categories);
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
		tr::binary_write(buffer, categories);
		tr::binary_write(buffer, playtime);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), tr::rand<std::uint8_t>(rng))};
		tr::binary_write(file, std::span{encrypted});
		LOG(tr::severity::INFO, "Saved scores to '{}'.", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save scores to '{}': {}.", path.string(), err.what());
	}
}