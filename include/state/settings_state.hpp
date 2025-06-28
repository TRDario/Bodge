#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Settings screen state.
class settings_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a settings screen state.
	settings_state(std::unique_ptr<game>&& game);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the settings menu state.
	enum class substate : std::uint8_t {
		// In the settings screen.
		IN_SETTINGS,
		// Exiting the settings screen.
		ENTERING_TITLE
	};

	// The current menu substate.
	substate _substate;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// Background game.
	std::unique_ptr<game> _game;
	// The pending settings.
	settings_t _pending;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};