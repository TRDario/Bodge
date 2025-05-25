#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Paused game state.
class pause_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{5};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a pause state.
	pause_state(active_game&& game, bool blur_in) noexcept;

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Gets the state type.
	u32 type() const noexcept override;
	// Handles an event.
	unique_ptr<state> handle_event(const tr::event& event) override;
	// Updates the state.
	unique_ptr<state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the main menu state.
	enum class substate : u8 {
		// Pausing the game.
		PAUSING,
		// The game is paused.
		PAUSED,
		// Unpausing the game.
		UNPAUSING,
		// Restarting the game.
		RESTARTING,
		// Saving and quitting the game.
		SAVING_AND_QUITTING,
		// Quitting the game to the title screen.
		QUITTING
	};

	// The current game substate.
	substate _substate;
	// The current tick timestamp.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// The paused game.
	active_game _game;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Gets the saturation factor to pass to the background shader.
	float saturation_factor() const noexcept;
	// Gets the blur strength to pass to the background shader.
	float blur_strength() const noexcept;

	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};