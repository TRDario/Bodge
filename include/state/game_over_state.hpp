#pragma once
#include "state.hpp"

class game_over_state : public state {
  public:
	game_over_state(std::unique_ptr<active_game>&& game, bool blur_in, ticks prev_pb);

	std::unique_ptr<tr::state> update(tr::duration) override;
	void draw() override;

  private:
	enum class substate {
		BLURRING_IN,
		GAME_OVER,
		SAVING_AND_RESTARTING,
		RESTARTING,
		SAVING_AND_QUITTING,
		QUITTING
	};

	substate m_substate;
	std::unique_ptr<active_game> m_game;
	ticks m_prev_pb;

	float shader_saturation_factor() const;
	float shader_blur_strength() const;

	void set_up_exit_animation();
};