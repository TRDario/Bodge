#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Replay game state.
class replay_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{7};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a replay state.
	replay_state(unique_ptr<replay_game>&& game, bool fade_in) noexcept;

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
		WATCHING,
		// Exiting into the replay screen.
		EXITING
	};

	// The current game substate.
	substate _substate;
	// The current tick timestamp.
	ticks _timer;
	// The gamestate.
	unique_ptr<replay_game> _game;
	// The UI manager.
	ui_manager _ui;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const noexcept;
	// Adds the replay cursor to the renderer.
	void add_cursor_to_renderer(vec2 pos) const;
};