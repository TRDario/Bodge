#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Paused replay state.
class replay_pause_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{8};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a pause state.
	replay_pause_state(unique_ptr<replay_game>&& game, bool blur_in) noexcept;

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
		// Pausing the replay.
		PAUSING,
		// The replay is paused.
		PAUSED,
		// Unpausing the replay.
		UNPAUSING,
		// Restarting the replay.
		RESTARTING,
		// Quitting the replay to the replays screen.
		QUITTING
	};

	// The current substate.
	substate _substate;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// The paused replay.
	unique_ptr<replay_game> _game;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Gets the saturation factor to pass to the background shader.
	float saturation_factor() const noexcept;
	// Gets the blur strength to pass to the background shader.
	float blur_strength() const noexcept;

	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};