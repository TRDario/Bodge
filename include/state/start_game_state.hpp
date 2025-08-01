#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// "Start Game" screen state.
class start_game_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates the start game state.
	start_game_state(std::unique_ptr<game>&& game);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the start game menu state.
	enum class substate : std::uint8_t {
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
	substate m_substate;
	// Internal timer.
	ticks m_timer;
	// The UI manager.
	ui_manager m_ui;
	// Background game.
	std::unique_ptr<game> m_background_game;
	// The list of available gamemodes.
	std::vector<gamemode> m_gamemodes;
	// The currently-selected gamemode.
	std::vector<gamemode>::iterator m_selected;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Sets up the exit animation.
	void set_up_exit_animation();
};