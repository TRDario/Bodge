#include "../include/score.hpp"
#include "../include/gamemode.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

std::strong_ordering operator<=>(const score& l, const score& r)
{
	return l.result <=> r.result;
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

///////////////////////////////////////////////////////////// SCORE CATEGORY //////////////////////////////////////////////////////////////

std::span<const std::byte> tr::binary_reader<score_category>::read_from_span(std::span<const std::byte> span, score_category& out)
{
	span = tr::binary_read(span, out.gamemode);
	span = tr::binary_read(span, out.pb);
	return tr::binary_read(span, out.scores);
}

void tr::binary_writer<score_category>::write_to_stream(std::ostream& os, const score_category& in)
{
	tr::binary_write(os, in.gamemode);
	tr::binary_write(os, in.pb);
	tr::binary_write(os, in.scores);
}

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

ticks scorefile_t::category_pb(const gamemode& gamemode) const
{
	std::vector<score_category>::const_iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gamemode; })};
	return it != categories.end() ? it->pb : 0;
}

void scorefile_t::update_category(const gamemode& gamemode, ticks pb)
{
	std::vector<score_category>::iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gamemode; })};
	if (it == categories.end()) {
		it = categories.insert(it, {gamemode, pb, {}});
	}
	else {
		it->pb = std::max(it->pb, pb);
	}
}

void scorefile_t::add_score(const gamemode& gamemode, const score& score)
{
	std::vector<score_category>::iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gamemode; })};
	if (it == categories.end()) {
		it = categories.insert(it, {gamemode, 0, {}});
	}
	it->scores.insert(std::upper_bound(it->scores.begin(), it->scores.end(), score, std::greater<>{}), score);
}

void scorefile_t::load_from_file()
{
	const std::filesystem::path path{cli_settings.userdir / "scorefile.dat"};
	try {
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		if (tr::binary_read<std::uint8_t>(file) != SCOREFILE_VERSION) {
			LOG(tr::severity::ERROR, "Failed to load scorefile.");
			LOG_CONTINUE("From: '{}'", path.string());
			LOG_CONTINUE("Invalid scorefile version.");
			return;
		}
		const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		data = tr::binary_read(data, name);
		data = tr::binary_read(data, categories);
		data = tr::binary_read(data, playtime);
		data = tr::binary_read(data, last_selected_gamemode);
		LOG(tr::severity::INFO, "Loaded scorefile.");
		LOG_CONTINUE("From: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load scorefile.");
		LOG_CONTINUE("From: '{}'", path.string());
		LOG_CONTINUE("{}", err.what());
	}
}

void scorefile_t::save_to_file()
{
	const std::filesystem::path path{cli_settings.userdir / "scorefile.dat"};
	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, name);
		tr::binary_write(buffer, categories);
		tr::binary_write(buffer, playtime);
		tr::binary_write(buffer, last_selected_gamemode);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), rng.generate<std::uint8_t>())};
		tr::binary_write(file, SCOREFILE_VERSION);
		tr::binary_write(file, std::span{encrypted});
		LOG(tr::severity::INFO, "Saved scorefile.");
		LOG_CONTINUE("To: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save scorefile.");
		LOG_CONTINUE("To: '{}'", path.string());
		LOG_CONTINUE("{}", err.what());
	}
}