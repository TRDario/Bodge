#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Replay game state.
class replay_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a replay state.
	replay_state(std::unique_ptr<replay_game>&& game, bool fade_in) noexcept;

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the replay state.
	enum class substate : std::uint8_t {
		// Transitioning into the game state from the menu.
		STARTING,
		// Regular operation.
		WATCHING,
		// Watching the game over animation.
		GAME_OVERING,
		// Exiting into the replay screen.
		EXITING
	};

	// The current game substate.
	substate _substate;
	// The current tick timestamp.
	ticks _timer;
	// The gamestate.
	std::unique_ptr<replay_game> _game;
	// The UI manager.
	ui_manager _ui;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const noexcept;
	// Adds the replay cursor to the renderer.
	void add_cursor_to_renderer(glm::vec2 pos) const;
};