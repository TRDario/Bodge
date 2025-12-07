#include "../include/score.hpp"
#include "../include/gamemode.hpp"
#include "../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr u8 SCOREFILE_VERSION{1};

////////////////////////////////////////////////////////////////// SCORE //////////////////////////////////////////////////////////////////

std::span<const std::byte> tr::binary_reader<score_entry>::read_from_span(std::span<const std::byte> span, score_entry& out)
{
	span = tr::binary_read(span, out.description);
	span = tr::binary_read(span, out.timestamp);
	span = tr::binary_read(span, out.score);
	span = tr::binary_read(span, out.time);
	return tr::binary_read(span, out.flags);
}

void tr::binary_writer<score_entry>::write_to_stream(std::ostream& os, const score_entry& in)
{
	tr::binary_write(os, in.description);
	tr::binary_write(os, in.timestamp);
	tr::binary_write(os, in.score);
	tr::binary_write(os, in.time);
	tr::binary_write(os, in.flags);
}

bool compare_scores(const score_entry& l, const score_entry& r)
{
	return l.score > r.score;
}

bool compare_times(const score_entry& l, const score_entry& r)
{
	return l.time > r.time;
}

///////////////////////////////////////////////////////////// SCORE CATEGORY //////////////////////////////////////////////////////////////

std::span<const std::byte> tr::binary_reader<score_category>::read_from_span(std::span<const std::byte> span, score_category& out)
{
	span = tr::binary_read(span, out.gamemode);
	span = tr::binary_read(span, out.best_score);
	span = tr::binary_read(span, out.best_time);
	return tr::binary_read(span, out.entries);
}

void tr::binary_writer<score_category>::write_to_stream(std::ostream& os, const score_category& in)
{
	tr::binary_write(os, in.gamemode);
	tr::binary_write(os, in.best_score);
	tr::binary_write(os, in.best_time);
	tr::binary_write(os, in.entries);
}

//////////////////////////////////////////////////////////////// SCOREFILE ////////////////////////////////////////////////////////////////

void scorefile::load_from_file()
{
	const std::filesystem::path path{g_cli_settings.user_directory / "scorefile.dat"};
	try {
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		if (tr::binary_read<u8>(file) == SCOREFILE_VERSION) {
			const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
			std::span<const std::byte> data{raw};
			data = tr::binary_read(data, name);
			data = tr::binary_read(data, categories);
			data = tr::binary_read(data, playtime);
			data = tr::binary_read(data, last_selected);
		}
	}
	catch (std::exception&) {
		return;
	}
}

void scorefile::save_to_file() const
{
	const std::filesystem::path path{g_cli_settings.user_directory / "scorefile.dat"};
	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, name);
		tr::binary_write(buffer, categories);
		tr::binary_write(buffer, playtime);
		tr::binary_write(buffer, last_selected);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), g_rng.generate<u8>())};
		tr::binary_write(file, SCOREFILE_VERSION);
		tr::binary_write(file, std::span{encrypted});
	}
	catch (std::exception&) {
		return;
	}
}

//

bests scorefile::bests(const gamemode& gm) const
{
	std::vector<score_category>::const_iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gm; })};
	return it != categories.end() ? ::bests{it->best_score, it->best_time} : ::bests{0, 0};
}

std::string scorefile::format_player_info() const
{
	return TR_FMT::format("{} {}: {}", g_loc["total_playtime"], name, format_playtime(playtime));
}

//

void scorefile::add_score(const gamemode& gm, const score_entry& s)
{
	std::vector<score_category>::iterator it{std::ranges::find_if(categories, [&](const auto& c) { return c.gamemode == gm; })};
	if (it == categories.end()) {
		it = categories.insert(it, {gm, s.score, s.time, {}});
	}
	else {
		it->best_score = std::max(it->best_score, s.score);
		it->best_time = std::max(it->best_time, s.time);
	}
	it->entries.emplace_back(s);
	playtime += s.time;
}