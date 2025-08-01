#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

enum class save_screen_flags : std::uint8_t {
	// No special flags.
	NONE = 0x0,
	// Flag signifying that the game is over.
	GAME_OVER = 0x4,
	// Flag signifying that the game will be restarted after saving.
	RESTARTING = 0x8,
	// Mask of the save screen flags.
	MASK = 0xC
};
DEFINE_BITMASK_OPERATORS(save_screen_flags);

// Score saving state.
class save_score_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a score saving state (from the pause screen).
	save_score_state(std::unique_ptr<active_game>&& game, glm::vec2 mouse_pos, save_screen_flags flags);
	// Creates a score saving state (from the game over screen).
	save_score_state(std::unique_ptr<active_game>&& game, ticks prev_pb, save_screen_flags flags);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// base substates within the score saving state.
	enum class substate_base : std::uint8_t {
		// Currently saving the score.
		SAVING_SCORE = 0x0,
		// Returning to the previous menu.
		RETURNING = 0x1,
		// Entering the replay saving sceen.
		ENTERING_SAVE_REPLAY = 0x2
	};
	// substate_base combined with save_screen_flags.
	enum class substate : std::uint8_t {
	};
	// Substate-specific data.
	union substate_data {
		// Stored mouse position for the pause screen state.
		glm::vec2 mouse_pos;
		// Stored previous personal best for the game over state.
		ticks prev_pb;
	};

	// The current menu substate.
	substate m_substate;
	// Substate-specific data.
	substate_data m_substate_data;
	// Internal timer.
	ticks m_timer;
	// The UI manager.
	ui_manager m_ui;
	// Background game.
	std::unique_ptr<active_game> m_game;
	// The score that will be saved.
	score m_score;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Combines a base substate and flags into a substate.
	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	// Converts a substate to a base substate.
	friend substate_base to_base(substate state);
	// Converts a substate to save screen flags.
	friend save_screen_flags to_flags(substate state);

	// Sets up the UI.
	void set_up_ui();
	// Sets up the exit animation.
	void set_up_exit_animation();
};