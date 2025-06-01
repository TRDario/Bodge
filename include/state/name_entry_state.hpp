#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Name entry state.
class name_entry_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{9};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a name entry state.
	name_entry_state();

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	u32 type() const noexcept override;
	unique_ptr<state> handle_event(const tr::event& event) override;
	unique_ptr<state> update(tr::duration) override;
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
	substate _substate;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// The background game.
	unique_ptr<game> _game;
};