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
	enum class substate_base : std::uint8_t {
		// Currently saving the score.
		SAVING_REPLAY = 0x0,
		// Entering the replay saving sceen.
		EXITING = 0x1
	};
	// substate_base combined with save_screen_flags.
	enum class substate : std::uint8_t {
	};

	// The current menu substate.
	substate m_substate;
	// Internal timer.
	ticks m_timer;
	// The UI manager.
	ui_manager m_ui;
	// Background game.
	std::unique_ptr<active_game> m_game;
	// Header of the replay that will be saved.
	replay_header m_replay;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Combines a base substate and flags into a substate.
	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	// Converts a substate to a base substate.
	friend substate_base to_base(substate state);
	// Converts a substate to save screen flags.
	friend save_screen_flags to_flags(substate state);
	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const;
	// Sets up the exit animation.
	void set_up_exit_animation();
};