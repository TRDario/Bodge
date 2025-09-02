#include "../include/gamemode.hpp"
#include "../include/legacy_formats.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"

namespace engine {
	void load_scorefile_v0(const std::filesystem::path& path, std::ifstream& file);
	void load_scorefile_v1(const std::filesystem::path& path, std::ifstream& file);
} // namespace engine

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr u8 SCOREFILE_VERSION{1};

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

std::strong_ordering operator<=>(const score_entry& l, const score_entry& r)
{
	return l.time <=> r.time;
}

std::span<const std::byte> tr::binary_reader<score_entry>::read_from_span(std::span<const std::byte> span, score_entry& out)
{
	span = tr::binary_read(span, out.description);
	span = tr::binary_read(span, out.unix_timestamp);
	span = tr::binary_read(span, out.score);
	span = tr::binary_read(span, out.time);
	return tr::binary_read(span, out.flags);
}

void tr::binary_writer<score_entry>::write_to_stream(std::ostream& os, const score_entry& in)
{
	tr::binary_write(os, in.description);
	tr::binary_write(os, in.unix_timestamp);
	tr::binary_write(os, in.score);
	tr::binary_write(os, in.time);
	tr::binary_write(os, in.flags);
}

///////////////////////////////////////////////////////////// SCORE CATEGORY //////////////////////////////////////////////////////////////

std::span<const std::byte> tr::binary_reader<score_category>::read_from_span(std::span<const std::byte> span, score_category& out)
{
	span = tr::binary_read(span, out.gamemode);
	span = tr::binary_read(span, out.best_score);
	span = tr::binary_read(span, out.best_time);
	return tr::binary_read(span, out.scores);
}

void tr::binary_writer<score_category>::write_to_stream(std::ostream& os, const score_category& in)
{
	tr::binary_write(os, in.gamemode);
	tr::binary_write(os, in.best_score);
	tr::binary_write(os, in.best_time);
	tr::binary_write(os, in.scores);
}

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

bests scorefile::bests(const gamemode& gm) const
{
	std::vector<score_category>::const_iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gm; })};
	return it != categories.end() ? ::bests{it->best_score, it->best_time} : ::bests{0, 0};
}

void scorefile::update_bests(const gamemode& gm, i64 score, ticks time)
{
	std::vector<score_category>::iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gm; })};
	if (it == categories.end()) {
		it = categories.insert(it, {gm, score, time, {}});
	}
	else {
		it->best_score = std::max(it->best_score, score);
		it->best_time = std::max(it->best_time, time);
	}
}

void scorefile::add_score(const gamemode& gm, const score_entry& s)
{
	std::vector<score_category>::iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gm; })};
	if (it == categories.end()) {
		it = categories.insert(it, {gm, 0, 0, {}});
	}
	it->scores.insert(std::upper_bound(it->scores.begin(), it->scores.end(), s, std::greater<>{}), s);
}

///////////////////////////////////////////////////////////////// ENGINE //////////////////////////////////////////////////////////////////

void engine::load_scorefile()
{
	const std::filesystem::path path{cli_settings.user_directory / "scorefile.dat"};
	try {
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		const u8 version{tr::binary_read<u8>(file)};
		switch (version) {
		case 0:
			load_scorefile_v0(path, file);
			return;
		case 1:
			load_scorefile_v1(path, file);
			return;
		default:
			LOG(tr::severity::ERROR, "Failed to load scorefile.");
			LOG_CONTINUE("From: '{}'", path.string());
			LOG_CONTINUE("Unsupported scorefile version {:d}.", version);
			return;
		}
		const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		data = tr::binary_read(data, scorefile.name);
		data = tr::binary_read(data, scorefile.categories);
		data = tr::binary_read(data, scorefile.playtime);
		data = tr::binary_read(data, scorefile.last_selected);
		LOG(tr::severity::INFO, "Loaded scorefile.");
		LOG_CONTINUE("From: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load scorefile.");
		LOG_CONTINUE("From: '{}'", path.string());
		LOG_CONTINUE(err);
	}
}

void engine::load_scorefile_v0(const std::filesystem::path& path, std::ifstream& file)
{
	const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
	std::span<const std::byte> data{raw};
	std::vector<score_category_v0> ignore;
	data = tr::binary_read(data, scorefile.name);
	data = tr::binary_read(data, ignore);
	data = tr::binary_read(data, scorefile.playtime);
	LOG(tr::severity::INFO, "Converted legacy scorefile (v0) to latest format.");
	LOG_CONTINUE("From: '{}'", path.string());
}

void engine::load_scorefile_v1(const std::filesystem::path& path, std::ifstream& file)
{
	const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
	std::span<const std::byte> data{raw};
	data = tr::binary_read(data, scorefile.name);
	data = tr::binary_read(data, scorefile.categories);
	data = tr::binary_read(data, scorefile.playtime);
	data = tr::binary_read(data, scorefile.last_selected);
	LOG(tr::severity::INFO, "Loaded scorefile.");
	LOG_CONTINUE("From: '{}'", path.string());
}

void engine::save_scorefile()
{
	const std::filesystem::path path{cli_settings.user_directory / "scorefile.dat"};
	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, scorefile.name);
		tr::binary_write(buffer, scorefile.categories);
		tr::binary_write(buffer, scorefile.playtime);
		tr::binary_write(buffer, scorefile.last_selected);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), rng.generate<u8>())};
		tr::binary_write(file, SCOREFILE_VERSION);
		tr::binary_write(file, std::span{encrypted});
		LOG(tr::severity::INFO, "Saved scorefile.");
		LOG_CONTINUE("To: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save scorefile.");
		LOG_CONTINUE("To: '{}'", path.string());
		LOG_CONTINUE(err);
	}
}