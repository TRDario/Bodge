#pragma once
#include "../game.hpp"
#include "../ui.hpp"
#include <future>

////////////////////////////////////////////////////////////////// STATE //////////////////////////////////////////////////////////////////

// Base class for all states.
class state : public tr::state {
  public:
	// Creates a state with an associated selection tree and shortcut table.
	state(selection_tree selection_tree, shortcut_table shortcuts);

	tr::next_state handle_event(const tr::sys::event& event) override;
	tr::next_state tick() override;

  protected:
	// State UI manager.
	ui_manager m_ui;
	// Time elapsed since the current substate began.
	ticks m_elapsed;
	// Holds the result of an asynchronous state load.
	std::future<tr::next_state> m_next_state;

	// Returns the next state if the elapsed time is greater than the given timestamp, otherwise return tr::KEEP_STATE.
	tr::next_state next_state_if_after(ticks timestamp);
};

///////////////////////////////////////////////////////////// MAIN MENU STATE /////////////////////////////////////////////////////////////

// Base class for main menu states.
class main_menu_state : public state {
  public:
	// Creates a main menu state with no prior background game.
	main_menu_state(selection_tree selection_tree, shortcut_table shortcuts);
	// Creates a main menu state with an existing background game.
	main_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<playerless_game> game);

	tr::next_state tick() override;
	void draw() override;

  protected:
	// Game playing in the background.
	std::shared_ptr<playerless_game> m_game;

  private:
	// The opacity of the fade overlay.
	virtual float fade_overlay_opacity();
};

///////////////////////////////////////////////////////////// GAME MENU STATE /////////////////////////////////////////////////////////////

// Whether a game menu state should update the game in the background.
enum class update_game : bool {
	NO,
	YES
};

// Base class for in-game menu states.
class game_menu_state : public state {
  public:
	// Creates a game menu state.
	game_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<game> game, update_game update_game);

	tr::next_state tick() override;
	void draw() override;

  protected:
	// Background game.
	std::shared_ptr<game> m_game;

  private:
	// Flag denoting whether to update the game in the background.
	bool m_update_game;

	// The saturation of the background game.
	virtual float saturation_factor();
	// The strength of the background blur.
	virtual float blur_strength();
	// The opacity of the fade overlay.
	virtual float fade_overlay_opacity();
};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

// Asynchronously creates a new state.
template <class T, class... Ts>
std::future<tr::next_state> make_async(Ts&&... args)
	requires(std::constructible_from<T, Ts...>)
{
	return std::async(
		std::launch::async, []<class... Us>(Us&&... args) { return (tr::next_state)std::make_unique<T>(std::forward<Us>(args)...); },
		std::forward<Ts>(args)...);
}