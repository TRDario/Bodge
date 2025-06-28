#include "../include/replay.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

void tr::binary_reader<replay_header>::read_from_stream(std::istream& is, replay_header& out)
{
	tr::binary_read<score>(is, out);
	tr::binary_read(is, out.name);
	tr::binary_read(is, out.player);
	tr::binary_read(is, out.gamemode);
	tr::binary_read(is, out.seed);
}

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

std::span<std::byte> tr::binary_writer<replay_header>::write_to_span(std::span<std::byte> span, const replay_header& in)
{
	span = tr::binary_write<score>(span, in);
	span = tr::binary_write(span, in.name);
	span = tr::binary_write(span, in.player);
	span = tr::binary_write(span, in.gamemode);
	return tr::binary_write(span, in.seed);
}

///////////////////////////////////////////////////////////////// REPLAY //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

replay::replay(const gamemode& gamemode, std::uint64_t seed) noexcept
	: _header{}
{
	_header.player = scorefile.name;
	_header.gamemode = gamemode;
	_header.seed = seed;
}

replay::replay(const std::string& filename)
{
	const std::filesystem::path path{cli_settings.userdir / "replays" / filename};
	std::vector<std::byte> encrypted;
	std::vector<std::byte> decrypted;
	std::ifstream file{tr::open_file_r(path, std::ios::binary)};

	tr::binary_read(file, encrypted);
	tr::decrypt_to(decrypted, encrypted);
	tr::binary_read(decrypted, _header);

	tr::binary_read(file, encrypted);
	tr::decrypt_to(decrypted, encrypted);
	tr::binary_read(decrypted, _inputs);
	_next = _inputs.begin();
	LOG(tr::severity::INFO, "Loaded replay '{}' from '{}'.", _header.name, path.string());
}

replay::replay(const replay& r)
	: _header{r._header}, _inputs{r._inputs}, _next{_inputs.begin()}
{
}

//////////////////////////////////////////////////////////////// RECORDING ////////////////////////////////////////////////////////////////

void replay::append(glm::vec2 input)
{
	_inputs.push_back(input);
}

void replay::set_header(const score& header, std::string_view name) noexcept
{
	static_cast<score&>(_header) = header;
	_header.name = name;
}

void replay::save_to_file() const noexcept
{
	std::filesystem::path path{cli_settings.userdir / "replays" / std::format("{}.dat", _header.name)};
	try {
		std::ofstream file;
		if (!std::filesystem::exists(path)) {
			file = tr::open_file_w(path, std::ios::binary);
		}
		else {
			int index{0};
			do {
				path = cli_settings.userdir / "replays" / std::format("{}({}).dat", _header.name, index++);
			} while (std::filesystem::exists(path));
			file = tr::open_file_w(path, std::ios::binary);
		}

		std::ostringstream bufstream{std::ios::binary};
		std::vector<std::byte> buffer;
		tr::binary_write(bufstream, _header);
		tr::encrypt_to(buffer, tr::range_bytes(bufstream.view()), tr::rand<std::uint8_t>(rng));
		tr::binary_write(file, buffer);

		bufstream.str({});
		tr::binary_write(bufstream, _inputs);
		tr::encrypt_to(buffer, tr::range_bytes(bufstream.view()), tr::rand<std::uint8_t>(rng));
		tr::binary_write(file, buffer);
		LOG(tr::severity::INFO, "Saved replay '{}' to '{}'.", _header.name, path.string());
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to save replay: {}", path.string(), err.what());
	}
}

//////////////////////////////////////////////////////////////// PLAYBACK /////////////////////////////////////////////////////////////////

const replay_header& replay::header() const noexcept
{
	return _header;
}

bool replay::done() const noexcept
{
	return _next == _inputs.end();
}

glm::vec2 replay::next() noexcept
{
	return *_next++;
}

glm::vec2 replay::current() const noexcept
{
	return done() ? *std::prev(_next) : *_next;
}

std::map<std::string, replay_header> load_replay_headers() noexcept
{
	std::map<std::string, replay_header> replays;
	try {
		const std::filesystem::path replay_dir{cli_settings.userdir / "replays"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{replay_dir}) {
			const std::filesystem::path path{file};
			if (!file.is_regular_file() || path.extension() != ".dat") {
				continue;
			}

			try {
				std::ifstream is{tr::open_file_r(file, std::ios::binary)};
				replays.emplace(path.filename(), tr::binary_read<replay_header>(tr::decrypt(tr::binary_read<std::vector<std::byte>>(is))));
				LOG(tr::severity::INFO, "Loaded replay header from '{}'.", path.string());
			}
			catch (std::exception& err) {
				LOG(tr::severity::ERROR, "Failed to load replay header from '{}': {}.", path.string(), err.what());
			}
		}
		LOG(tr::severity::INFO, "Loaded replay headers.");
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to load replay headers: {}.", err.what());
	}
	return replays;
}