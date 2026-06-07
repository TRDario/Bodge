///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements score.hpp.                                                                                                                 //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/score.hpp"
#include "../include/gamemode.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Savefile version identifier.
constexpr u8 SAVEFILE_VERSION{2};

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

///////////////////////////////////////////////////////////////// SAVEFILE ////////////////////////////////////////////////////////////////

savefile::savefile(const std::filesystem::path& path)
{
	try {
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		const u8 version{tr::binary_read<u8>(file)};
		if (version == SAVEFILE_VERSION) {
			const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
			std::span<const std::byte> data{raw};
			data = tr::binary_read(data, m_name);
			data = tr::binary_read(data, m_score_categories);
			data = tr::binary_read(data, m_playtime);
			data = tr::binary_read(data, gamemode_draft);
			data = tr::binary_read(data, last_selected_gamemode);
		}
	}
	catch (std::exception&) {
		return;
	}
}

//

void savefile::save_to_file(const std::filesystem::path& path)
{
	// Don't save unnamed savefile.
	if (unnamed()) {
		return;
	}

	try {
		std::ofstream file{tr::open_file_w(path, std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, m_name);
		tr::binary_write(buffer, m_score_categories);
		tr::binary_write(buffer, m_playtime);
		tr::binary_write(buffer, gamemode_draft);
		tr::binary_write(buffer, last_selected_gamemode);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), g_rng.generate<u8>())};
		tr::binary_write(file, SAVEFILE_VERSION);
		tr::binary_write(file, std::span{encrypted});
	}
	catch (std::exception&) {
		return;
	}
}

//

bool savefile::unnamed() const
{
	return m_name.empty();
}

std::string_view savefile::name() const
{
	return m_name;
}

void savefile::rename(std::string_view name)
{
	m_name = name;
}

//

const std::vector<score_category>& savefile::score_categories() const
{
	return m_score_categories;
}

best_results savefile::best_results(const gamemode& gm) const
{
	std::vector<score_category>::const_iterator category_it{std::ranges::find(m_score_categories, gm, &score_category::gamemode)};
	return category_it != m_score_categories.end() ? ::best_results{category_it->best_score, category_it->best_time} : ::best_results{0, 0};
}

void savefile::add_score(const gamemode& gm, const score_entry& s)
{
	std::vector<score_category>::iterator category_it{
		std::ranges::find_if(m_score_categories, [&](const auto& c) { return c.gamemode == gm; })};
	if (category_it == m_score_categories.end()) {
		category_it = m_score_categories.insert(category_it, {gm, s.score, s.time, {}});
	}
	else {
		category_it->best_score = std::max(category_it->best_score, s.score);
		category_it->best_time = std::max(category_it->best_time, s.time);
	}
	category_it->entries.emplace_back(s);
	m_playtime += s.time;
}

//

std::string savefile::format_info(const localization& localization) const
{
	return TR_FMT::format("{} {}: {}", localization["total_playtime"], m_name, format_playtime(m_playtime));
}