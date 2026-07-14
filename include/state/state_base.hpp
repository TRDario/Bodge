///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides base classes for game states.                                                                                                //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../audio.hpp"
#include "../game.hpp"
#include "../input.hpp"
#include "../renderer.hpp"
#include "../ui.hpp"
#include <future>

////////////////////////////////////////////////////////////////// STATE //////////////////////////////////////////////////////////////////

// Base class for all states.
class state : public tr::state {
  public:
	// Application subsystems.
	struct subsystems {
		// Initializes the application subsystems.
		subsystems();

		// Active settings.
		settings settings;
		// Input manager.
		input input;
		// Localization manager.
		localization localization;
		// Audio manager.
		audio audio;
		// Renderer.
		renderer renderer;
	};

	// Creates a state with an associated selection tree and shortcut table.
	state(std::shared_ptr<subsystems> subsystems, selection_tree selection_tree, shortcut_table shortcuts);

	// Handles an event.
	tr::next_state handle_event(const tr::sys::event& event) override;
	// Updates the state.
	tr::next_state tick() override;
	// Draws the state.
	void draw() override final;

  protected:
	// Cursor types.
	enum class cursor_type : bool {
		opaque,
		transparent
	};

	// Application subsystems.
	std::shared_ptr<subsystems> m_subsystems;
	// State UI manager.
	ui_manager m_ui;
	// Time elapsed since the current substate began.
	ticks m_elapsed;
	// Holds the result of an asynchronous state load.
	std::future<tr::next_state> m_next_state;

	// Returns the next state if the elapsed time is greater than the given timestamp, otherwise return tr::KEEP_STATE.
	tr::next_state next_state_if_after(ticks timestamp);

	// Adds the cursor to the renderer.
	void add_cursor_to_renderer(cursor_type type);

  private:
	// Inner drawing function.
	virtual void draw_game() = 0;
};

///////////////////////////////////////////////////////////// MAIN MENU STATE /////////////////////////////////////////////////////////////

// Whether a screen's title should be animated.
enum class animate_title : bool {
	NO,
	YES
};

// Whether a screen's subtitle should be animated.
enum class animate_subtitle : bool {
	NO,
	YES
};

// Base class for main menu states.
class main_menu_state : public state {
  public:
	// Creates a main menu state with no prior background game.
	main_menu_state(std::shared_ptr<subsystems> subsystems, selection_tree selection_tree, shortcut_table shortcuts);
	// Creates a main menu state with an existing background game.
	main_menu_state(std::shared_ptr<subsystems> subsystems, selection_tree selection_tree, shortcut_table shortcuts,
					std::shared_ptr<playerless_game> game);

	// Updates the state.
	tr::next_state tick() override;

  protected:
	// Game playing in the background.
	std::shared_ptr<playerless_game> m_game;

  private:
	// Gets the opacity of the fade overlay.
	virtual float fade_overlay_opacity() const;
	// Gets the type of cursor to draw.
	virtual cursor_type cursor_type() const;

	// Draws the state.
	void draw_game() override;
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
	game_menu_state(std::shared_ptr<subsystems> subsystems, selection_tree selection_tree, shortcut_table shortcuts,
					std::shared_ptr<game> game, savefile savefile, update_game update_game);

	// Updates the state.
	tr::next_state tick() override;

  protected:
	// Background game.
	std::shared_ptr<game> m_game;
	// Cached copy of the savefile.
	savefile m_savefile;

  private:
	// Flag denoting whether to update the game in the background.
	bool m_update_game;

	// Gets the saturation of the background game.
	virtual float saturation_factor() const;
	// Gets the strength of the background blur.
	virtual float blur_strength() const;
	// Gets the opacity of the fade overlay.
	virtual float fade_overlay_opacity() const;
	// Gets the type of cursor to draw.
	virtual cursor_type cursor_type() const;

	// Draws the state.
	void draw_game() override;
};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

// Asynchronously creates a new state.
template <class T, class... Ts>
std::future<tr::next_state> make_async(Ts... args)
	requires(std::constructible_from<T, Ts...>)
{
	constexpr auto constructor{[](auto... args) { return (tr::next_state)std::make_unique<T>(std::move(args)...); }};
	return std::async(std::launch::async, constructor, std::move(args)...);
}