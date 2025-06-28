#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Title screen state.
class title_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a title state with a randomly chosen background gamemode.
	title_state();
	// Creates a title state with an ongoing game.
	title_state(std::unique_ptr<game>&& game);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the title screen state.
	enum class substate : std::uint8_t {
		// Entering the game.
		ENTERING_GAME,
		// In the main menu.
		IN_TITLE,
		// Entering the "Start Game" menu.
		ENTERING_START_GAME,
		// Entering the replays screen.
		ENTERING_REPLAYS,
		// Entering the scores screen.
		ENTERING_SCOREBOARDS,
		// Entering the settings screen.
		ENTERING_SETTINGS,
		// Exiting the game.
		EXITING_GAME
	};

	// The current menu substate.
	substate _substate;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// Background game.
	std::unique_ptr<game> _game;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const noexcept;
	// Sets up the UI.
	void set_up_ui();
	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};