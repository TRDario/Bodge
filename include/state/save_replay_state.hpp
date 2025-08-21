#pragma once
#include "save_score_state.hpp"

class save_replay_state : public state {
  public:
	save_replay_state(std::unique_ptr<active_game>&& game, save_screen_flags flags);

	std::unique_ptr<tr::state> update(tr::duration) override;
	void draw() override;

  private:
	enum class substate_base {
		SAVING_REPLAY = 0x0,
		EXITING = 0x1
	};
	enum class substate {
	}; // substate_base + save_screen_flags

	substate m_substate;
	std::unique_ptr<active_game> m_game;
	replay_header m_replay;

	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	friend substate_base to_base(substate state);
	friend save_screen_flags to_flags(substate state);

	float fade_overlay_opacity() const;
	void set_up_exit_animation();
};