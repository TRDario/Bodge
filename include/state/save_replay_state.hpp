#pragma once
#include "save_score_state.hpp"

// Replay saving state.
class save_replay_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a replay saving state.
	save_replay_state(std::unique_ptr<active_game>&& game, save_screen_flags flags);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// base substates within the replay saving state.
	enum class base_substate : std::uint8_t {
		// Currently saving the score.
		SAVING_REPLAY = 0x0,
		// Entering the replay saving sceen.
		EXITING = 0x1
	};
	// base_substate combined with save_screen_flags.
	enum class substate : std::uint8_t {
	};

	// The current menu substate.
	substate _substate;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// Background game.
	std::unique_ptr<active_game> _game;
	// Header of the replay that will be saved.
	replay_header _replay;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Combines a base substate and flags into a substate.
	friend substate operator|(const base_substate& l, const save_screen_flags& r) noexcept;
	// Converts a substate to a base substate.
	friend base_substate to_base(substate state) noexcept;
	// Converts a substate to save screen flags.
	friend save_screen_flags to_flags(substate state) noexcept;
	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const noexcept;
	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};