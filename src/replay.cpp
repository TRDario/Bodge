#include "../include/replay.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

void tr::binary_reader<replay_header>::read_from_stream(istream& is, replay_header& out)
{
	binary_read<score>(is, out);
	binary_read(is, out.name);
	binary_read(is, out.player);
	binary_read(is, out.gamemode);
	binary_read(is, out.seed);
}

span<const byte> tr::binary_reader<replay_header>::read_from_span(span<const byte> span, replay_header& out)
{
	span = binary_read<score>(span, out);
	span = binary_read(span, out.name);
	span = binary_read(span, out.player);
	span = binary_read(span, out.gamemode);
	return binary_read(span, out.seed);
}

void tr::binary_writer<replay_header>::write_to_stream(ostream& os, const replay_header& in)
{
	binary_write<score>(os, in);
	binary_write(os, in.name);
	binary_write(os, in.player);
	binary_write(os, in.gamemode);
	binary_write(os, in.seed);
}

span<byte> tr::binary_writer<replay_header>::write_to_span(span<byte> span, const replay_header& in)
{
	span = binary_write<score>(span, in);
	span = binary_write(span, in.name);
	span = binary_write(span, in.player);
	span = binary_write(span, in.gamemode);
	return binary_write(span, in.seed);
}

///////////////////////////////////////////////////////////////// REPLAY //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

replay::replay(const gamemode& gamemode, std::uint64_t seed) noexcept
	: _header{.name = {}, .player = scorefile.name, .gamemode = gamemode, .seed = seed}
{
}

replay::replay(const string& filename)
{
	const path path{cli_settings.userdir / "replays" / filename};
	vector<byte> encrypted;
	vector<byte> decrypted;
	ifstream file{open_file_r(path, std::ios::binary)};

	binary_read(file, encrypted);
	tr::decrypt_to(decrypted, encrypted);
	binary_read(decrypted, _header);

	binary_read(file, encrypted);
	tr::decrypt_to(decrypted, encrypted);
	binary_read(decrypted, _inputs);
	_next = _inputs.begin();
	LOG(INFO, "Loaded replay '{}' from '{}'.", _header.name, path.string());
}

replay::replay(const replay& r)
	: _header{r._header}, _inputs{r._inputs}, _next{_inputs.begin()}
{
}

//////////////////////////////////////////////////////////////// RECORDING ////////////////////////////////////////////////////////////////

void replay::append(vec2 input)
{
	_inputs.push_back(input);
}

void replay::set_header(score&& header, string_view name) noexcept
{
	static_cast<score&>(_header) = std::move(header);
	_header.name = std::move(name);
}

void replay::save_to_file() noexcept
{
	const path base_path{cli_settings.userdir / "replays" / (_header.name + ".dat")};
	try {
		ofstream file;
		if (!exists(base_path)) {
			file = open_file_w(base_path, std::ios::binary);
		}
		else {
			int index{0};
			path path{cli_settings.userdir / "replays" / format("{}({}).dat", _header.name, index++)};
			while (exists(path)) {
				path = cli_settings.userdir / "replays" / format("{}({}).dat", _header.name, index++);
			}
			file = open_file_w(path, std::ios::binary);
		}

		std::ostringstream bufstream{std::ios::binary};
		vector<byte> buffer;
		binary_write(bufstream, _header);
		encrypt_to(buffer, range_bytes(bufstream.view()), rand<u8>(rng));
		binary_write(file, buffer);

		bufstream.str({});
		binary_write(bufstream, _inputs);
		encrypt_to(buffer, range_bytes(bufstream.view()), rand<u8>(rng));
		binary_write(file, buffer);
		LOG(INFO, "Saved replay '{}' to '{}'.", _header.name, base_path.string());
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to save replay: {}", base_path.string(), err.what());
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

vec2 replay::next() noexcept
{
	return *_next++;
}

vec2 replay::current() const noexcept
{
	return done() ? *std::prev(_next) : *_next;
}

map<string, replay_header> load_replay_headers() noexcept
{
	map<string, replay_header> replays;
	try {
		const path replay_dir{cli_settings.userdir / "replays"};
		for (directory_entry file : directory_iterator{replay_dir}) {
			const path path{file};
			if (!file.is_regular_file() || path.extension() != ".dat") {
				continue;
			}

			try {
				ifstream is{open_file_r(file, std::ios::binary)};
				replays.emplace(path.filename(), binary_read<replay_header>(decrypt(binary_read<vector<byte>>(is))));
				LOG(INFO, "Loaded replay header from '{}'.", path.string());
			}
			catch (std::exception& err) {
				LOG(ERROR, "Failed to load replay header from '{}': {}.", path.string(), err.what());
			}
		}
		LOG(INFO, "Loaded replay headers.");
	}
	catch (std::exception& err) {
		LOG(ERROR, "Failed to load replay headers: {}.", err.what());
	}
	return replays;
}