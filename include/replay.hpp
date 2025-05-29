#pragma once
#include "score.hpp"

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

// The replay header.
struct replay_header : score {
	// The name of the replay.
	string name;
	// The player who created the replay.
	string player;
	// The game gamemode.
	gamemode gamemode;
	// The game seed.
	std::uint64_t seed;
};

// Replay header binary reader.
template <> struct tr::binary_reader<replay_header> {
	static void read_from_stream(istream& is, replay_header& out);
	static span<const byte> read_from_span(span<const byte> span, replay_header& out);
};

// Replay header binary writer.
template <> struct tr::binary_writer<replay_header> {
	static void write_to_stream(ostream& os, const replay_header& in);
	static span<byte> write_to_span(span<byte> span, const replay_header& in);
};

////////////////////////////////////////////////////////////////// REPLAY /////////////////////////////////////////////////////////////////

// Game recording.
class replay {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates an empty replay.
	replay(const gamemode& gamemode, std::uint64_t seed) noexcept;
	// Loads a replay from file.
	replay(const string& filename);

	////////////////////////////////////////////////////////////// RECORDING //////////////////////////////////////////////////////////////

	// Appends an input to the replay.
	void append(vec2 input);
	// Sets the replay header.
	void set_header(score&& score, string&& name) noexcept;
	// Saves the replay to file.
	void save_to_file() noexcept;

	////////////////////////////////////////////////////////////// PLAYBACK ///////////////////////////////////////////////////////////////

	// Gets the replay header.
	const replay_header& header() const noexcept;
	// Gets whether the replay is done.
	bool done() const noexcept;
	// Gets the next input.
	vec2 next() noexcept;
	// Gets the current input.
	vec2 current() const noexcept;

  private:
	// The replay header.
	replay_header _header;
	// The inputs of the replay.
	vector<vec2> _inputs;
	// Iterator to the next input.
	vector<vec2>::iterator _next;
};

// Loads all found replay headers.
map<string, replay_header> load_replay_headers() noexcept;