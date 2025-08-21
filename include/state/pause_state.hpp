#pragma once
#include "state.hpp"

class pause_state : public state {
  public:
	pause_state(std::unique_ptr<game>&& game, game_type type, glm::vec2 mouse_pos, bool blur_in);

	std::unique_ptr<tr::state> update(tr::duration) override;
	void draw() override;

  private:
	enum class substate_base {
		PAUSING,
		PAUSED,
		UNPAUSING,
		SAVING_AND_RESTARTING,
		RESTARTING,
		SAVING_AND_QUITTING,
		QUITTING,
	};
	enum class substate {
	}; // substate_base + game_type

	substate m_substate;
	std::unique_ptr<game> m_game;
	glm::vec2 m_start_mouse_pos;
	glm::vec2 m_end_mouse_pos;

	friend substate operator|(const substate_base& l, const game_type& r);
	friend substate_base to_base(substate state);
	friend game_type to_type(substate state);

	float shader_saturation_factor() const;
	float shader_blur_strength() const;

	void set_up_full_ui();
	void set_up_limited_ui();
	void set_up_exit_animation();
};