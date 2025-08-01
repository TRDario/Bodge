#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Game over state.
class game_over_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a game over state.
	game_over_state(std::unique_ptr<active_game>&& game, bool blur_in, ticks prev_pb);

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the game over state.
	enum class substate : std::uint8_t {
		// Transitioning into the game over state from the game.
		BLURRING_IN,
		// The game is over.
		GAME_OVER,
		// Saving and restarting the game.
		SAVING_AND_RESTARTING,
		// Restarting the game.
		RESTARTING,
		// Saving and quitting the game.
		SAVING_AND_QUITTING,
		// Quitting the game to the title screen.
		QUITTING
	};

	// The current game substate.
	substate m_substate;
	// The current tick timestamp.
	ticks m_timer;
	// The UI manager.
	ui_manager m_ui;
	// The gamestate.
	std::unique_ptr<active_game> m_game;
	// Cached information about the previous personal best.
	ticks m_prev_pb;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Gets the saturation factor to pass to the background shader.
	float saturation_factor() const;
	// Gets the blur strength to pass to the background shader.
	float blur_strength() const;

	// Sets up the exit animation.
	void set_up_exit_animation();
};