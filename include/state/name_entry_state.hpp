#pragma once
#include "state.hpp"

class name_entry_state : public menu_state {
  public:
	name_entry_state();

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		FADING_IN,
		IN_NAME_ENTRY,
		ENTERING_TITLE
	};

	substate m_substate;

	float fade_overlay_opacity() override;
};