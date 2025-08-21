#pragma once
#include "state.hpp"

class ball_settings_editor_state : public menu_state {
  public:
	ball_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_EDITOR,
		EXITING
	};

	substate m_substate;
	gamemode m_pending;

	void set_up_exit_animation();
};