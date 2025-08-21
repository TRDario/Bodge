#pragma once
#include "state.hpp"

class gamemode_designer_state : public menu_state {
  public:
	gamemode_designer_state(std::unique_ptr<game>&& game, const gamemode& gamemode, bool returning_from_subscreen);
	gamemode_designer_state(const gamemode& gamemode);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		RETURNING_FROM_TEST_GAME,
		IN_GAMEMODE_DESIGNER,
		ENTERING_TEST_GAME,
		ENTERING_BALL_SETTINGS_EDITOR,
		ENTERING_PLAYER_SETTINGS_EDITOR,
		ENTERING_TITLE
	};

	substate m_substate;
	gamemode m_pending;
	std::vector<std::string> m_available_songs;

	float fade_overlay_opacity() override;
	void set_up_ui(bool returning_from_subscreen);
	void set_up_subscreen_animation();
	void set_up_exit_animation();
};