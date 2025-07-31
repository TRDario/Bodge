#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Replay screen state.
class replays_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a replay screen state with a randomly chosen background gamemode.
	replays_state();
	// Creates a replay screen state with an ongoing game.
	replays_state(std::unique_ptr<game>&& game);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the replay screen state.
	enum class substate : std::uint8_t {
		// Entering the replays screen from a replay.
		RETURNING_FROM_REPLAY,
		// In the replays screen.
		IN_REPLAYS,
		// Switching the page.
		SWITCHING_PAGE,
		// Starting a replay.
		STARTING_REPLAY,
		// Exiting the replays screen.
		ENTERING_TITLE
	};

	// The current menu substate.
	substate m_substate;
	// The current replay page.
	std::uint16_t m_page;
	// Internal timer.
	ticks m_timer;
	// The UI manager.
	ui_manager m_ui;
	// Background game.
	std::unique_ptr<game> m_background_game;
	// List of replays.
	std::map<std::string, replay_header> m_replays;
	// The selected replay.
	std::map<std::string, replay_header>::iterator m_selected;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const;
	// Sets up the UI.
	void set_up_ui();
	// Sets up the page switching animation.
	void set_up_page_switch_animation();
	// Sets up the exit animation.
	void set_up_exit_animation();
};