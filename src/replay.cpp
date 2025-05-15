#include "../include/replay.hpp"
#include "../include/settings.hpp"

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

void tr::binary_reader<replay_header>::read_from_stream(std::istream& is, replay_header& out)
{
	binary_read<score>(is, out);
	binary_read(is, out.name);
	binary_read(is, out.player);
	binary_read(is, out.seed);
}

std::span<const std::byte> tr::binary_reader<replay_header>::read_from_span(std::span<const std::byte> span, replay_header& out)
{
	span = binary_read<score>(span, out);
	span = binary_read(span, out.name);
	span = binary_read(span, out.player);
	return binary_read(span, out.seed);
}

void tr::binary_writer<replay_header>::write_to_stream(std::ostream& os, const replay_header& in)
{
	binary_write<score>(os, in);
	binary_write(os, in.name);
	binary_write(os, in.player);
	binary_write(os, in.seed);
}

std::span<std::byte> tr::binary_writer<replay_header>::write_to_span(std::span<std::byte> span, const replay_header& in)
{
	span = binary_write<score>(span, in);
	span = binary_write(span, in.name);
	span = binary_write(span, in.player);
	return binary_write(span, in.seed);
}

///////////////////////////////////////////////////////////////// REPLAY //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

replay::replay(std::uint64_t seed) noexcept
	: _header{.player = scorefile.name, .seed = seed}
{
}

replay::replay(const std::string& filename)
{
	const std::filesystem::path path{cli_settings.userdir / "replays" / filename};
	std::vector<std::byte> encrypted;
	std::vector<std::byte> decrypted;
	std::ifstream file{tr::open_file_r(path, std::ios::binary)};

	tr::binary_read(file, encrypted);
	tr::decrypt_to(encrypted, decrypted);
	tr::binary_read(decrypted, _header);

	tr::binary_read(file, encrypted);
	tr::decrypt_to(encrypted, decrypted);
	tr::binary_read(decrypted, _inputs);
	LOG(tr::severity::INFO, "Loaded replay '{}' from '{}'.", _header.name, path.string());
}

//////////////////////////////////////////////////////////////// RECORDING ////////////////////////////////////////////////////////////////

void replay::append(glm::vec2 input)
{
	_inputs.push_back(input);
}

void replay::set_header(score&& header, std::string&& name) noexcept
{
	static_cast<score&>(_header) = std::move(header);
	_header.name = std::move(name);
}

void replay::save_to_file() noexcept
{
	const std::filesystem::path path{cli_settings.userdir / "replays" / (_header.name + ".rpy")};
	try {
		const std::filesystem::path base_path{cli_settings.userdir / "replays" / (_header.name + ".rpy")};
		std::ofstream file;
		if (!exists(base_path)) {
			file = tr::open_file_w(base_path, std::ios::binary);
		}
		else {
			int index{0};
			std::filesystem::path path{cli_settings.userdir / "replays" / std::format("{}({}).rpy", _header.name, index++)};
			while (exists(path)) {
				path = cli_settings.userdir / "replays" / std::format("{}({}).rpy", _header.name, index++);
			}
		}

		std::stringstream bufstream;
		std::vector<std::byte> buffer;
		tr::binary_write(bufstream, _header);
		tr::encrypt_to(tr::range_bytes(bufstream.view()), rand<std::uint8_t>(rng), buffer);
		tr::binary_write(file, buffer);

		bufstream.clear();
		tr::binary_write(bufstream, _inputs);
		tr::encrypt_to(tr::range_bytes(bufstream.view()), rand<std::uint8_t>(rng), buffer);
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

std::map<std::string, replay_header> load_replay_headers() noexcept
{
	std::map<std::string, replay_header> replays;
	try {
		const std::filesystem::path replay_dir{cli_settings.userdir / "replays"};
		for (std::filesystem::directory_entry file : std::filesystem::directory_iterator{replay_dir}) {
			const std::filesystem::path path{file};
			if (!file.is_regular_file() || path.extension() != ".rpy") {
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