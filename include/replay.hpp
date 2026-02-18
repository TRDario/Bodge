///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides replay structures and functions relating to them.                                                                            //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "score.hpp"

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

// Game replay header containing metadata.
struct replay_header : score_entry {
	// The name of the replay.
	tr::static_string<16 * 4> name;
	// The name of the player who created the replay.
	tr::static_string<20 * 4> player;
	// The gamemode of the replay.
	gamemode gamemode;
	// The seed of the replay.
	u64 seed;
};
template <> struct tr::binary_reader<replay_header> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, replay_header& out);
};
template <> struct tr::binary_writer<replay_header> {
	static void write_to_stream(std::ostream& os, const replay_header& in);
};

////////////////////////////////////////////////////////////////// REPLAY /////////////////////////////////////////////////////////////////

// Game replay information.
class replay {
  public:
	// Creates an empty replay.
	replay(const gamemode& gamemode, u64 seed);
	// Loads a replay from file.
	replay(const std::string& filename);
	replay(const replay& r);
	replay(replay&&) noexcept = default;

	// Appends an input to the replay.
	void append(glm::vec2 input);
	// Sets the replay's header.
	void set_header(const score_entry& score, std::string_view name);
	// Saves the replay to a file based on its name.
	void save_to_file() const;

	// Gets the replay's header.
	const replay_header& header() const;
	// Gets whether the replay is done playing.
	bool done() const;
	// Gets the next input in the replay.
	glm::vec2 next_input();
	// Gets the previous input in the replay.
	glm::vec2 prev_input() const;

  private:
	// The replay's header.
	replay_header m_header;
	// List of player inputs.
	std::vector<glm::vec2> m_inputs;
	// Iterator to the next input to return.
	std::vector<glm::vec2>::iterator m_next_it;
};

// Map of available replays.
using replay_map = std::map<std::string, replay_header>;
// Loads all available replay headers.
std::map<std::string, replay_header> load_replay_headers();