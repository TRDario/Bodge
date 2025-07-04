#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Paused game state.
class pause_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a pause state.
	pause_state(std::unique_ptr<game>&& game, game_type type, glm::vec2 mouse_pos, bool blur_in) noexcept;

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Base substates within the pause state.
	enum class substate_base : std::uint8_t {
		// Pausing the game.
		PAUSING,
		// The game is paused.
		PAUSED,
		// Unpausing the game.
		UNPAUSING,
		// Saving and restarting the game.
		SAVING_AND_RESTARTING,
		// Restarting the game.
		RESTARTING,
		// Saving and quitting the game.
		SAVING_AND_QUITTING,
		// Quitting the game.
		QUITTING,
	};
	// Substates of the pause screen.
	enum class substate : std::uint8_t {
	};

	// The current game substate.
	substate _substate;
	// The current tick timestamp.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// The paused game.
	std::unique_ptr<game> _game;
	// The position of the mouse right before pausing.
	glm::vec2 _start_mouse_pos;
	// The position of the mouse right before unpausing.
	glm::vec2 _end_mouse_pos;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Combines a substate base and game type into a substate.
	friend substate operator|(const substate_base& l, const game_type& r) noexcept;
	// Converts a substate to a substate base.
	friend substate_base to_base(substate state) noexcept;
	// Converts a substate to a game type.
	friend game_type to_type(substate state) noexcept;

	// Gets the saturation factor to pass to the background shader.
	float saturation_factor() const noexcept;
	// Gets the blur strength to pass to the background shader.
	float blur_strength() const noexcept;

	// Sets up the full pause UI.
	void set_up_full_ui();
	// Sets up the limited pause UI.
	void set_up_limited_ui();
	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};