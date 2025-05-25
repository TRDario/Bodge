#pragma once
#include "../game/game.hpp"

// Active game state.
class game_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{4};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a game state.
	game_state(active_game&& game, bool fade_in) noexcept;

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
		// Transitioning into the game state from the menu.
		STARTING,
		// Regular operation.
		PLAYING,
	};

	// The current game substate.
	substate _substate;
	// The current tick timestamp.
	ticks _timer;
	// The gamestate.
	active_game _game;
};