#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// "Start Game" screen state.
class start_game_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{1};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates the start game state.
	start_game_state(unique_ptr<game>&& game) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	u32 type() const noexcept override;
	unique_ptr<state> handle_event(const tr::event& event) override;
	unique_ptr<state> update(tr::duration) override;
	void draw() override;

  private:
	// Substates within the main menu state.
	enum class substate : u8 {
		// In the start game menu.
		IN_START_GAME,
		// In the start game menu, switching a gamemode.
		SWITCHING_GAMEMODE,
		// Exiting the start game menu.
		ENTERING_TITLE,
		// Entering the game.
		ENTERING_GAME
	};

	// The current menu substate.
	substate _substate;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// Background game.
	unique_ptr<game> _game;
	// The list of available gamemodes.
	vector<gamemode> _gamemodes;
	// The currently-selected gamemode.
	vector<gamemode>::iterator _cur;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};