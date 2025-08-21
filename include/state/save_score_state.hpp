#pragma once
#include "../game/game.hpp"
#include "state.hpp"

enum class save_screen_flags : std::uint8_t {
	NONE = 0x0,
	GAME_OVER = 0x4,
	RESTARTING = 0x8,
	MASK = 0xC
};
DEFINE_BITMASK_OPERATORS(save_screen_flags);

class save_score_state : public state {
  public:
	// Used when coming from the pause screen.
	save_score_state(std::unique_ptr<active_game>&& game, glm::vec2 mouse_pos, save_screen_flags flags);
	// Used when coming from the game over screen.
	save_score_state(std::unique_ptr<active_game>&& game, ticks prev_pb, save_screen_flags flags);

	std::unique_ptr<tr::state> update(tr::duration) override;
	void draw() override;

  private:
	enum class substate_base {
		SAVING_SCORE = 0x0,
		RETURNING = 0x1,
		ENTERING_SAVE_REPLAY = 0x2
	};
	enum class substate {
	}; // substate_base + save_screen_flags
	union substate_data {
		glm::vec2 start_mouse_pos;
		ticks prev_pb;
	};

	substate m_substate;
	substate_data m_substate_data;
	std::unique_ptr<active_game> m_game;
	score m_score;

	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	friend substate_base to_base(substate state);
	friend save_screen_flags to_flags(substate state);

	void set_up_ui();
	void set_up_exit_animation();
};