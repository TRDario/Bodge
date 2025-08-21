#pragma once
#include "state.hpp"

class credits_state : public menu_state {
  public:
	credits_state(std::unique_ptr<game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_CREDITS,
		ENTERING_TITLE,
	};

	substate m_substate;
};