#pragma once
#include "state.hpp"

class start_game_state : public menu_state {
  public:
	start_game_state(std::unique_ptr<game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		ENTERING_START_GAME,
		IN_START_GAME,
		SWITCHING_GAMEMODE,
		ENTERING_TITLE,
		ENTERING_GAME
	};

	substate m_substate;
	std::vector<gamemode> m_gamemodes;
	std::vector<gamemode>::iterator m_selected;

	float fade_overlay_opacity() override;
	void set_up_exit_animation();
};