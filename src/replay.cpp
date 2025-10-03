#include "../include/replay.hpp"
#include "../include/settings.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Replay file version identifier.
constexpr u8 REPLAY_VERSION{1};

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

std::span<const std::byte> tr::binary_reader<replay_header>::read_from_span(std::span<const std::byte> span, replay_header& out)
{
	span = tr::binary_read<score_entry>(span, out);
	span = tr::binary_read(span, out.name);
	span = tr::binary_read(span, out.player);
	span = tr::binary_read(span, out.gamemode);
	return tr::binary_read(span, out.seed);
}

void tr::binary_writer<replay_header>::write_to_stream(std::ostream& os, const replay_header& in)
{
	tr::binary_write<score_entry>(os, in);
	tr::binary_write(os, in.name);
	tr::binary_write(os, in.player);
	tr::binary_write(os, in.gamemode);
	tr::binary_write(os, in.seed);
}

///////////////////////////////////////////////////////////////// REPLAY //////////////////////////////////////////////////////////////////

replay::replay(const gamemode& gamemode, u64 seed)
	: m_header{}
{
	m_header.player = engine::scorefile.name;
	m_header.gamemode = gamemode;
	m_header.seed = seed;
}

replay::replay(const std::string& filename)
{
	const std::filesystem::path path{engine::cli_settings.user_directory / "replays" / filename};
	std::vector<std::byte> encrypted;
	std::vector<std::byte> decrypted;
	std::ifstream file{tr::open_file_r(path, std::ios::binary)};

	std::ignore = tr::binary_read<u8>(file);

	tr::binary_read(file, encrypted);
	tr::decrypt_to(decrypted, encrypted);
	tr::binary_read(decrypted, m_header);

	tr::binary_read(file, encrypted);
	tr::decrypt_to(decrypted, encrypted);
	tr::binary_read(decrypted, m_inputs);
	m_next_it = m_inputs.begin();
}

replay::replay(const replay& r)
	: m_header{r.m_header}, m_inputs{r.m_inputs}, m_next_it{m_inputs.begin()}
{
}

//

void replay::append(glm::vec2 input)
{
	m_inputs.push_back(input);
}

void replay::set_header(const score_entry& header, std::string_view name)
{
	(score_entry&)(m_header) = header;
	m_header.name = name;
}

std::string to_filename(std::string_view name)
{
	std::string filename{name};
	std::erase_if(filename, [](char chr) { return chr >= 0x7F || (!std::isalnum(chr) && chr != '_' && chr != '-' && chr != ' '); });
	std::ranges::for_each(filename, [](char& chr) { chr = std::tolower(chr); });
	std::ranges::replace(filename, ' ', '_');
	return filename.empty() ? "replay" : filename;
}

void replay::save_to_file() const
{
	try {
		std::string filename{to_filename(m_header.name)};
		std::filesystem::path path{engine::cli_settings.user_directory / "replays" / TR_FMT::format("{}.dat", filename)};
		std::ofstream file;
		if (!std::filesystem::exists(path)) {
			file = tr::open_file_w(path, std::ios::binary);
		}
		else {
			int index{0};
			do {
				path = engine::cli_settings.user_directory / "replays" / TR_FMT::format("{}({}).dat", filename, index++);
			} while (std::filesystem::exists(path));
			file = tr::open_file_w(path, std::ios::binary);
		}

		tr::binary_write(file, REPLAY_VERSION);

		std::ostringstream bufstream{std::ios::binary};
		std::vector<std::byte> buffer;

		tr::binary_write(bufstream, m_header);
		tr::encrypt_to(buffer, bufstream.view(), engine::rng.generate<u8>());
		tr::binary_write(file, buffer);

		bufstream.str({});
		tr::binary_write(bufstream, m_inputs);
		tr::encrypt_to(buffer, bufstream.view(), engine::rng.generate<u8>());
		tr::binary_write(file, buffer);
	}
	catch (std::exception&) {
		return;
	}
}

//

const replay_header& replay::header() const
{
	return m_header;
}

bool replay::done() const
{
	return m_next_it == m_inputs.end();
}

glm::vec2 replay::next_input()
{
	return *m_next_it++;
}

glm::vec2 replay::prev_input() const
{
	return done() ? *std::prev(m_next_it) : *m_next_it;
}

std::map<std::string, replay_header> engine::load_replay_headers()
{
	std::map<std::string, replay_header> replays;
	try {
		const std::filesystem::path replay_dir{engine::cli_settings.user_directory / "replays"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{replay_dir}) {
			if (!file.is_regular_file() || file.path().extension() != ".dat") {
				continue;
			}

			try {
				std::ifstream is{tr::open_file_r(file, std::ios::binary)};
				const u8 version{tr::binary_read<u8>(is)};
				if (version != REPLAY_VERSION) {
					continue;
				}
				replays.emplace(file.path().filename().string(),
								tr::binary_read<replay_header>(tr::decrypt(tr::binary_read<std::vector<std::byte>>(is))));
			}
			catch (std::exception&) {
				continue;
			}
		}
	}
	catch (std::exception&) {
		return replays;
	}
	return replays;
}