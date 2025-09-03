#pragma once
#include "score.hpp"

////////////////////////////////////////////////////////////// REPLAY HEADER //////////////////////////////////////////////////////////////

struct replay_header : score_entry {
	tr::static_string<16 * 4> name;
	tr::static_string<20 * 4> player;
	gamemode gamemode;
	u64 seed;
};
template <> struct tr::binary_reader<replay_header> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, replay_header& out);
};
template <> struct tr::binary_writer<replay_header> {
	static void write_to_stream(std::ostream& os, const replay_header& in);
};

////////////////////////////////////////////////////////////////// REPLAY /////////////////////////////////////////////////////////////////

class replay {
  public:
	replay(const gamemode& gamemode, u64 seed); // Creates an empty replay.
	replay(const std::string& filename);        // Loads a replay from file.
	replay(const replay& r);
	replay(replay&&) noexcept = default;

	void append(glm::vec2 input);
	void set_header(const score_entry& score, std::string_view name);
	void save_to_file() const;

	const replay_header& header() const;
	bool done() const;
	glm::vec2 next_input();
	glm::vec2 prev_input() const;

  private:
	replay_header m_header;
	std::vector<glm::vec2> m_inputs;
	std::vector<glm::vec2>::iterator m_next_it;
};

namespace engine {
	std::map<std::string, replay_header> load_replay_headers();
} // namespace engine