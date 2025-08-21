#pragma once
#include "state.hpp"

class game_state : public state {
  public:
	game_state(std::unique_ptr<game>&& game, game_type type, bool fade_in);

	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	std::unique_ptr<tr::state> update(tr::duration) override;
	void draw() override;

  private:
	enum class substate_base {
		FADING_IN,
		ONGOING,
		GAME_OVER,
		EXITING
	};
	enum class substate {
	}; // substate_base + game_type.

	substate m_substate;
	std::unique_ptr<game> m_game;

	friend substate operator|(const substate_base& l, const game_type& r);
	friend substate_base to_base(substate state);
	friend game_type to_type(substate state);

	float fade_overlay_opacity() const;
	void add_replay_cursor_to_renderer(glm::vec2 pos) const;
};