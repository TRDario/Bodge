#pragma once
#include "state.hpp"

class settings_state : public menu_state {
  public:
	settings_state(std::unique_ptr<game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_SETTINGS,
		ENTERING_TITLE
	};

	substate m_substate;
	settings m_pending;

	void set_up_exit_animation();
};