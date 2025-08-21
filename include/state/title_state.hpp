#pragma once
#include "state.hpp"

class title_state : public menu_state {
  public:
	title_state();
	title_state(std::unique_ptr<game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		ENTERING_GAME,
		IN_TITLE,
		ENTERING_START_GAME,
		ENTERING_GAMEMODE_DESIGNER,
		ENTERING_REPLAYS,
		ENTERING_SCOREBOARDS,
		ENTERING_SETTINGS,
		ENTERING_CREDITS,
		EXITING_GAME
	};

	substate m_substate;

	float fade_overlay_opacity() override;
	void set_up_ui();
	void set_up_exit_animation();
};