#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Scores screen state.
class scoreboards_state : public tr::state {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a scores screen state with an ongoing game.
	scoreboards_state(std::unique_ptr<game>&& game);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	// Handles an event.
	std::unique_ptr<tr::state> handle_event(const tr::event& event) override;
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Substates within the score screen state.
	enum class substate : std::uint8_t {
		// In the scores screen.
		IN_SCORES,
		// Switching the page.
		SWITCHING_PAGE,
		// Exiting the scores screen.
		EXITING_TO_TITLE
	};

	// The current screen substate.
	substate _substate;
	// The current score page.
	std::uint16_t _page;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// Background game.
	std::unique_ptr<game> _game;
	// Iterator to the currently selected gamemode.
	std::vector<score_category>::iterator _current;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Sets up the page switching animation.
	void set_up_page_switch_animation() noexcept;
	// Sets up the exit animation.
	void set_up_exit_animation() noexcept;
};