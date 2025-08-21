#pragma once
#include "state.hpp"

class replays_state : public menu_state {
  public:
	replays_state();
	replays_state(std::unique_ptr<game>&& game);

  public:
	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		RETURNING_FROM_REPLAY,
		IN_REPLAYS,
		SWITCHING_PAGE,
		STARTING_REPLAY,
		ENTERING_TITLE
	};

	substate m_substate;
	std::uint16_t m_page;
	std::map<std::string, replay_header> m_replays;
	std::map<std::string, replay_header>::iterator m_selected;

	float fade_overlay_opacity() override;
	void set_up_ui();
	void set_up_page_switch_animation();
	void set_up_exit_animation();
};