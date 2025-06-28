#pragma once
#include "score.hpp"

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

// The replay header.
struct replay_header : score {
	// The name of the replay.
	tr::static_string<20> name;
	// The player who created the replay.
	tr::static_string<20> player;
	// The game gamemode.
	gamemode gamemode;
	// The game seed.
	std::uint64_t seed;
};

// Replay header binary reader.
template <> struct tr::binary_reader<replay_header> {
	static void read_from_stream(std::istream& is, replay_header& out);
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, replay_header& out);
};

// Replay header binary writer.
template <> struct tr::binary_writer<replay_header> {
	static void write_to_stream(std::ostream& os, const replay_header& in);
	static std::span<std::byte> write_to_span(std::span<std::byte> span, const replay_header& in);
};

////////////////////////////////////////////////////////////////// REPLAY /////////////////////////////////////////////////////////////////

// Game recording.
class replay {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates an empty replay.
	replay(const gamemode& gamemode, std::uint64_t seed) noexcept;
	// Loads a replay from file.
	replay(const std::string& filename);
	// Copies a replay.
	replay(const replay& r);
	// Moves a replay.
	replay(replay&&) noexcept = default;

	////////////////////////////////////////////////////////////// RECORDING //////////////////////////////////////////////////////////////

	// Appends an input to the replay.
	void append(glm::vec2 input);
	// Sets the replay header.
	void set_header(const score& score, std::string_view name) noexcept;
	// Saves the replay to file.
	void save_to_file() const noexcept;

	////////////////////////////////////////////////////////////// PLAYBACK ///////////////////////////////////////////////////////////////

	// Gets the replay header.
	const replay_header& header() const noexcept;
	// Gets whether the replay is done.
	bool done() const noexcept;
	// Gets the next input.
	glm::vec2 next() noexcept;
	// Gets the current input.
	glm::vec2 current() const noexcept;

  private:
	// The replay header.
	replay_header _header;
	// The inputs of the replay.
	std::vector<glm::vec2> _inputs;
	// Iterator to the next input.
	std::vector<glm::vec2>::iterator _next;
};

// Loads all found replay headers.
std::map<std::string, replay_header> load_replay_headers() noexcept;