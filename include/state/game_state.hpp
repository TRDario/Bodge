#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Active game state.
class game_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a game state.
	game_state(std::unique_ptr<game>&& game, game_type type, bool fade_in);

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Base substates within the game state.
	enum class substate_base : std::uint8_t {
		// Entering from another screen.
		STARTING,
		// Regular operation.
		ONGOING,
		// The game is over.
		GAME_OVER,
		// Exiting into another screen.
		EXITING
	};
	// Substates of the game state.
	enum class substate : std::uint8_t {
	};

	// The current game substate.
	substate m_substate;
	// The current tick timestamp.
	ticks m_timer;
	// The UI manager.
	ui_manager m_ui;
	// The actual game.
	std::unique_ptr<game> m_game;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Combines a substate base and game type into a substate.
	friend substate operator|(const substate_base& l, const game_type& r);
	// Converts a substate to a substate base.
	friend substate_base to_base(substate state);
	// Converts a substate to a game type.
	friend game_type to_type(substate state);

	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const;

	// Adds the replay cursor to the renderer.
	void add_replay_cursor_to_renderer(glm::vec2 pos) const;
};