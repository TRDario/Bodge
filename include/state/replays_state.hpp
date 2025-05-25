#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Replay screen state.
class replays_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{2};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a replay screen state with a randomly chosen background gamemode.
	replays_state();
	// Creates a replay screen state with an ongoing game.
	replays_state(game&& game);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	u32 type() const noexcept override;
	unique_ptr<state> handle_event(const tr::event& event) override;
	unique_ptr<state> update(tr::duration) override;
	void draw() override;

  private:
	// Substates within the main menu state.
	enum class substate : u8 {
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
	substate _substate;
	// The current replay page.
	u16 _page;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// Background game.
	game _game;
	// List of replays.
	map<string, replay_header> _replays;
	// The selected replay.
	map<string, replay_header>::iterator _selected;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Calculates the fade overlay opacity.
	float fade_overlay_opacity() const noexcept;
	// Sets up the UI.
	void set_up_ui();
	// Sets up the page switching animation.
	void set_up_page_switch_animation() noexcept;
	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};