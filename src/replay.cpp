#include "../include/replay.hpp"
#include "../include/settings.hpp"

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

std::string to_filename(std::string_view name)
{
	std::string filename{name};
	std::erase_if(filename, [](char chr) { return chr >= 0x7F || (!std::isalnum(chr) && chr != '_' && chr != '-'); });
	std::ranges::replace(filename, ' ', '_');
	return filename.empty() ? "Replay" : filename;
}

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

std::span<const std::byte> tr::binary_reader<replay_header>::read_from_span(std::span<const std::byte> span, replay_header& out)
{
	span = tr::binary_read<score>(span, out);
	span = tr::binary_read(span, out.name);
	span = tr::binary_read(span, out.player);
	span = tr::binary_read(span, out.gamemode);
	return tr::binary_read(span, out.seed);
}

void tr::binary_writer<replay_header>::write_to_stream(std::ostream& os, const replay_header& in)
{
	tr::binary_write<score>(os, in);
	tr::binary_write(os, in.name);
	tr::binary_write(os, in.player);
	tr::binary_write(os, in.gamemode);
	tr::binary_write(os, in.seed);
}

///////////////////////////////////////////////////////////////// REPLAY //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

replay::replay(const gamemode& gamemode, std::uint64_t seed)
	: m_header{}
{
	m_header.player = std::string_view{engine::scorefile.name};
	m_header.gamemode = gamemode;
	m_header.seed = seed;
}

replay::replay(const std::string& filename)
{
	const std::filesystem::path path{engine::cli_settings.userdir / "replays" / filename};
	std::vector<std::byte> encrypted;
	std::vector<std::byte> decrypted;
	std::ifstream file{tr::open_file_r(path, std::ios::binary)};

	tr::binary_read(file, encrypted);
	tr::decrypt_to(decrypted, encrypted);
	tr::binary_read(decrypted, m_header);

	tr::binary_read(file, encrypted);
	tr::decrypt_to(decrypted, encrypted);
	tr::binary_read(decrypted, m_inputs);
	m_next_it = m_inputs.begin();
	LOG(tr::severity::INFO, "Loaded replay '{}'.", m_header.name);
	LOG_CONTINUE("From: '{}'", path.string());
}

replay::replay(const replay& r)
	: m_header{r.m_header}, m_inputs{r.m_inputs}, m_next_it{m_inputs.begin()}
{
}

//////////////////////////////////////////////////////////////// RECORDING ////////////////////////////////////////////////////////////////

void replay::append(glm::vec2 input)
{
	m_inputs.push_back(input);
}

void replay::set_header(const score& header, std::string_view name)
{
	static_cast<score&>(m_header) = header;
	m_header.name = name;
}

void replay::save_to_file() const
{
	try {
		std::string filename{to_filename(m_header.name)};
		std::filesystem::path path{engine::cli_settings.userdir / "replays" / std::format("{}.dat", filename)};
		std::ofstream file;
		if (!std::filesystem::exists(path)) {
			file = tr::open_file_w(path, std::ios::binary);
		}
		else {
			int index{0};
			do {
				path = engine::cli_settings.userdir / "replays" / std::format("{}({}).dat", filename, index++);
			} while (std::filesystem::exists(path));
			file = tr::open_file_w(path, std::ios::binary);
		}

		std::ostringstream bufstream{std::ios::binary};
		std::vector<std::byte> buffer;
		tr::binary_write(bufstream, m_header);
		tr::encrypt_to(buffer, bufstream.view(), engine::rng.generate<std::uint8_t>());
		tr::binary_write(file, buffer);

		bufstream.str({});
		tr::binary_write(bufstream, m_inputs);
		tr::encrypt_to(buffer, bufstream.view(), engine::rng.generate<std::uint8_t>());
		tr::binary_write(file, buffer);
		LOG(tr::severity::INFO, "Saved replay '{}'.", m_header.name);
		LOG_CONTINUE("To: '{}'", path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save replay:");
		LOG_CONTINUE(err);
	}
}

//////////////////////////////////////////////////////////////// PLAYBACK /////////////////////////////////////////////////////////////////

const replay_header& replay::header() const
{
	return m_header;
}

bool replay::done() const
{
	return m_next_it == m_inputs.end();
}

glm::vec2 replay::next()
{
	return *m_next_it++;
}

glm::vec2 replay::current() const
{
	return done() ? *std::prev(m_next_it) : *m_next_it;
}

std::map<std::string, replay_header> load_replay_headers()
{
	std::map<std::string, replay_header> replays;
	try {
		const std::filesystem::path replay_dir{engine::cli_settings.userdir / "replays"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{replay_dir}) {
			const std::filesystem::path path{file};
			if (!file.is_regular_file() || path.extension() != ".dat") {
				continue;
			}

			try {
				std::ifstream is{tr::open_file_r(file, std::ios::binary)};
				replays.emplace(path.filename().string(),
								tr::binary_read<replay_header>(tr::decrypt(tr::binary_read<std::vector<std::byte>>(is))));
				LOG(tr::severity::INFO, "Loaded replay header.");
				LOG_CONTINUE("From: '{}'", path.string());
			}
			catch (std::exception& err) {
				LOG(tr::severity::ERROR, "Failed to load replay header.");
				LOG_CONTINUE("From: '{}'", path.string());
				LOG_CONTINUE(err);
			}
		}
		LOG(tr::severity::INFO, "Loaded replay headers.");
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load replay headers.");
		LOG_CONTINUE(err);
	}
	return replays;
}