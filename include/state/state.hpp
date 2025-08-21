#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

// Base bodge state.
class state : public tr::state {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	state(selection_tree selection_tree, shortcut_table shortcuts);

	//////////////////////////////////////////////////////////// VIRTUAL METHODS //////////////////////////////////////////////////////////

	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	std::unique_ptr<tr::state> update(tr::duration) override;

  protected:
	// The UI manager.
	ui_manager m_ui;
	// Internal timer.
	ticks m_timer;
};

// Base bodge menu state.
class menu_state : public state {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Constructs a menu state with a new background game.
	menu_state(selection_tree selection_tree, shortcut_table shortcuts);
	// Constructs a menu state with an existing background game.
	menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::unique_ptr<game>&& game);

	//////////////////////////////////////////////////////////////// METHODS //////////////////////////////////////////////////////////////

	// Releases the game from state ownership.
	std::unique_ptr<game>&& release_game();

	//////////////////////////////////////////////////////////// VIRTUAL METHODS //////////////////////////////////////////////////////////

  protected:
	// Returns the fade overlay opacity when drawing.
	virtual float fade_overlay_opacity();

  public:
	// Updates the state.
	std::unique_ptr<tr::state> update(tr::duration) override;
	// Draws the state.
	void draw() override;

  private:
	// Background game.
	std::unique_ptr<game> m_background_game;
};