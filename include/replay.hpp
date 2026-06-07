///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides replay structures and functions relating to them.                                                                            //
//                                                                                                                                       //
// Replays are loaded from replay files (a binary format) in <USER DIRECTORY>/replays which essentially contain a list of player inputs. //
// They are very sensitive to desynchronisation because all the actual game logic is repeated on the viewing side, and even something    //
// like compiling with the wrong compiler may return in miniscule gameplay differences that end up messing with replay playback.         //
// All official builds of Bodge are compiled with Clang 20 to ensure replay coherency across operating systems and some versions.        //
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
	replay(std::string_view player, const gamemode& gamemode, u64 seed);
	// Loads a replay from file.
	replay(const std::filesystem::path& path);
	replay(const replay& r);
	replay(replay&&) noexcept = default;

	// Appends an input to the replay.
	void append(glm::vec2 input);
	// Sets the replay's header.
	void set_header(const score_entry& score, std::string_view name);
	// Saves the replay to a file based on its name.
	void save_to_directory(const std::filesystem::path& directory = debug_settings::instance().user_directory() / "replays") const;

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
using replay_map = std::map<std::filesystem::path, replay_header>;
// Loads all available replay headers.
replay_map load_replay_headers(const std::filesystem::path& directory = debug_settings::instance().user_directory() / "replays");