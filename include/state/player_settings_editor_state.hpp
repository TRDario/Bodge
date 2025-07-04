#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Gamemode player settings editor state.
class player_settings_editor_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a gamemode player settings editor state.
	player_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the player settings editor state.
	enum class substate : std::uint8_t {
		// In the player settings editor screen.
		IN_EDITOR,
		// Exiting the player settings editor screen.
		EXITING
	};

	// The current menu substate.
	substate _substate;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// Background game.
	std::unique_ptr<game> _game;
	// The pending gamemode.
	gamemode _gamemode;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};