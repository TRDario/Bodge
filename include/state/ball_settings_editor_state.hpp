#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Gamemode player settings editor state.
class ball_settings_editor_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a gamemode ball settings editor state.
	ball_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the ball settings editor state.
	enum class substate : std::uint8_t {
		// In the ball settings editor screen.
		IN_EDITOR,
		// Exiting the ball settings editor screen.
		EXITING
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

	// Sets up the exit animation.
	void set_up_exit_animation();
};