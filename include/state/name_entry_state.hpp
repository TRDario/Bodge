#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Name entry state.
class name_entry_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a name entry state.
	name_entry_state();

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates of the anem entry state.
	enum class substate {
		// Entering the game.
		ENTERING_GAME,
		// In the name entry screen.
		IN_NAME_ENTRY,
		// Entering the title screen.
		ENTERING_TITLE
	};

	// The current substate of the menu.
	substate m_substate;
	// Internal timer.
	ticks m_timer;
	// The UI manager.
	ui_manager m_ui;
	// The background game.
	std::unique_ptr<game> m_background_game;
};