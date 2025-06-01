#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Settings screen state.
class settings_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{4};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a settings screen state.
	settings_state(unique_ptr<game>&& game);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	u32 type() const noexcept override;
	unique_ptr<state> handle_event(const tr::event& event) override;
	unique_ptr<state> update(tr::duration) override;
	void draw() override;

  private:
	// Substates within the main menu state.
	enum class substate : u8 {
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
	unique_ptr<game> _game;
	// The pending settings.
	settings_t _pending;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};