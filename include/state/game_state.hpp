#pragma once
#include "../game/game.hpp"

// Active game state.
class game_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a game state.
	game_state(std::unique_ptr<active_game>&& game, bool fade_in) noexcept;

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the game state.
	enum class substate : std::uint8_t {
		// Transitioning into the game state from the menu.
		STARTING,
		// Regular operation.
		PLAYING,
		// The game is over.
		GAME_OVER
	};

	// The current game substate.
	substate _substate;
	// The current tick timestamp.
	ticks _timer;
	// The gamestate.
	std::unique_ptr<active_game> _game;
};