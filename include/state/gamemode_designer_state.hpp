#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Gamemode editor screen state.
class gamemode_designer_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a gamemode editor screen state.
	gamemode_designer_state(std::unique_ptr<game>&& game, const gamemode& gamemode, bool returning_from_subscreen);
	// Creates a gamemode editor screen state returning from a test game.
	gamemode_designer_state(const gamemode& gamemode);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the gamemode editor state.
	enum class substate : std::uint8_t {
		// Returning from a test game.
		RETURNING_FROM_TEST_GAME,
		// In the gamemode editor screen.
		IN_GAMEMODE_DESIGNER,
		// Entering the test game.
		ENTERING_TEST_GAME,
		// Entering the ball settings editor subscreen.
		ENTERING_BALL_SETTINGS_EDITOR,
		// Entering the player settings editor subscreen.
		ENTERING_PLAYER_SETTINGS_EDITOR,
		// Exiting the gamemode editor screen.
		ENTERING_TITLE
	};

	// The current menu substate.
	substate m_substate;
	// Internal timer.
	ticks m_timer;
	// The UI manager.
	ui_manager m_ui;
	// Background game.
	std::unique_ptr<game> m_background_game;
	// The pending gamemode.
	gamemode m_pending;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const;

	// Sets up the UI.
	void set_up_ui(bool returning_from_subscreen);
	// Sets up the animation for entering a sub-screen.
	void set_up_subscreen_animation();
	// Sets up the exit animation.
	void set_up_exit_animation();
};