#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Scores screen state.
class scores_state : public state {
  public:
	////////////////////////////////////////////////////////////// CONSTANTS //////////////////////////////////////////////////////////////

	constexpr static u32 ID{3};

	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a scores screen state with an ongoing game.
	scores_state(game&& game);

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	u32 type() const noexcept override;
	unique_ptr<state> handle_event(const tr::event& event) override;
	unique_ptr<state> update(tr::duration) override;
	void draw() override;

  private:
	// Substates within the scores screen state.
	enum class substate : u8 {
		// In the scores screen.
		IN_SCORES,
		// Switching the gamemode.
		SWITCHING_GAMEMODE,
		// Switching the page.
		SWITCHING_PAGE,
		// Exiting the scores screen.
		EXITING_TO_TITLE
	};

	// The current screen substate.
	substate _substate;
	// The current score page.
	u16 _page;
	// Internal timer.
	ticks _timer;
	// The UI manager.
	ui_manager _ui;
	// Background game.
	game _game;
	// Iterator to the currently selected gamemode.
	unordered_map<gamemode, vector<score>>::iterator _current;
};