#pragma once
#include "state.hpp"

class scoreboards_state : public menu_state {
  public:
	scoreboards_state(std::unique_ptr<game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_SCOREBOARDS,
		SWITCHING_PAGE,
		EXITING_TO_TITLE
	};

	substate m_substate;
	int m_page;
	std::vector<score_category>::iterator m_selected;

	void set_up_page_switch_animation();
	void set_up_exit_animation();
};