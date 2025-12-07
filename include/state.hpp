#pragma once
#include "state/state_base.hpp"

////////////////////////////////////////////////////////////// STATE MACHINE //////////////////////////////////////////////////////////////

// The global state machine.
inline tr::state_machine g_state_machine;

//////////////////////////////////////////////////////////// NAME ENTRY STATE /////////////////////////////////////////////////////////////

// Name entry screen of the main menu that appears on first launch.
class name_entry_state : public main_menu_state {
  public:
	// Creates the name entry state.
	name_entry_state();

	tr::next_state tick() override;

  private:
	// Substates of the name entry state.
	enum class substate {
		// Fading in after launching the game.
		FADING_IN,
		// In the name entry screen.
		IN_NAME_ENTRY,
		// Confirmed and moving to title screen.
		EXITING
	};

	// The current substate.
	substate m_substate;

	float fade_overlay_opacity() override;
};

/////////////////////////////////////////////////////////////// TITLE STATE ///////////////////////////////////////////////////////////////

// Title screen of the main menu.
class title_state : public main_menu_state {
  public:
	// Creates a title state when starting.
	title_state();
	// Creates a title state when entering from the name entry state.
	title_state(std::shared_ptr<playerless_game> game);

	tr::next_state tick() override;

  private:
	// Substates of the title state.
	enum class substate {
		// Fading in after launching the game.
		FADING_IN,
		// In the title screen or entering it from a submenu.
		IN_TITLE,
		// Entering a submenu.
		EXITING_TO_SUBMENU,
		// Fading out before exiting the game.
		EXITING_GAME
	};

	// The current substate.
	substate m_substate;

	float fade_overlay_opacity() override;

	// Sets up the UI when entering.
	void set_up_ui();
	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////////// START GAME STATE /////////////////////////////////////////////////////////////

// Start game screen of the main menu.
class start_game_state : public main_menu_state {
  public:
	// Creates a start game state.
	start_game_state(std::shared_ptr<playerless_game> game);

	tr::next_state tick() override;

  private:
	// Substates of the start game state.
	enum class substate {
		// Entering the start game screen from the title screen.
		ENTERING_START_GAME,
		// In the start game screen.
		IN_START_GAME,
		// Switching to a different gamemode.
		SWITCHING_GAMEMODE,
		// Exiting back to the title screen.
		EXITING_TO_TITLE,
		// Starting a game.
		STARTING_GAME
	};

	// The current substate.
	substate m_substate;
	// List of available gamemodes.
	std::vector<gamemode> m_gamemodes;
	// The currently selected gamemode.
	std::vector<gamemode>::iterator m_selected;
	// Holds the result of an asynchronously loaded new set of widgets.
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	float fade_overlay_opacity() override;

	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

///////////////////////////////////////////////////////// GAMEMODE DESIGNER STATE /////////////////////////////////////////////////////////

// Whether a state is being entered from a subscreen.
enum class returning_from_subscreen : bool {
	NO,
	YES
};

// Gamemode designer screen of the main menu.
class gamemode_designer_state : public main_menu_state {
  public:
	// Creates a gamemode designer state coming from another menu state.
	gamemode_designer_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode,
							returning_from_subscreen returning_from_subscreen);
	// Creates a gamemode designer state coming from a test game.
	gamemode_designer_state(const gamemode& gamemode);

	tr::next_state tick() override;

  private:
	// Substates of the gamemode designer state.
	enum class substate {
		// Fading in and returning to the gamemode designer screen from a test game.
		RETURNING_FROM_TEST_GAME,
		// In the gamemode designer screen.
		IN_GAMEMODE_DESIGNER,
		// Fading out and entering a test game.
		ENTERING_TEST_GAME,
		// Exiting to either a subscreen or the title screen.
		EXITING,
	};

	// The current substate.
	substate m_substate;
	// The pending gamemode.
	gamemode m_pending;
	// List of names of available songs.
	std::vector<std::string> m_available_songs;

	float fade_overlay_opacity() override;

	// Sets up the UI when entering.
	void set_up_ui(returning_from_subscreen returning_from_subscreen);
	// Sets up the UI exit animation when entering a subscreen.
	void set_up_subscreen_animation();
	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////// BALL SETTINGS EDITOR STATE ///////////////////////////////////////////////////////

// Ball settings editor screen of the gamemode designer menu.
class ball_settings_editor_state : public main_menu_state {
  public:
	// Creates a ball settings editor state.
	ball_settings_editor_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode);

	tr::next_state tick() override;

  private:
	// Substates of the ball settings editor state.
	enum class substate {
		// In the ball settings editor screen or entering it.
		IN_EDITOR,
		// Exiting to the gamemode designer screen.
		EXITING
	};

	// The current substate.
	substate m_substate;
	// The pending gamemode.
	gamemode m_pending;

	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

/////////////////////////////////////////////////////// PLAYER SETTINGS EDITOR STATE //////////////////////////////////////////////////////

// Player settings editor screen of the gamemode designer menu.
class player_settings_editor_state : public main_menu_state {
  public:
	// Creates a player settings editor state.
	player_settings_editor_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode);

	tr::next_state tick() override;

  private:
	// Substates of the player settings editor state.
	enum class substate {
		// In the player settings editor screen or entering it.
		IN_EDITOR,
		// Exiting to the gamemode designer screen.
		EXITING
	};

	// The current substate.
	substate m_substate;
	// The pending gamemode.
	gamemode m_pending;

	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

//

// Scoreboard selection screen of the main menu.
class scoreboard_selection_state : public main_menu_state {
  public:
	// Creates a scoreboard selection state.
	scoreboard_selection_state(std::shared_ptr<playerless_game> game, returning_from_subscreen returning_from_subscreen);

	tr::next_state tick() override;

  private:
	// Substates of the scoreboard selection state.
	enum class substate {
		// In the scoreboard selection screen or entering it.
		IN_SCOREBOARD_SELECTION,
		// Exiting to one of the scoreboard screens or the title screen.
		EXITING,
	};

	// The current substate.
	substate m_substate;

	// Sets up the UI exit animation when entering a subscreen.
	void set_up_subscreen_animation();
	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////////// SCOREBOARDS STATE ////////////////////////////////////////////////////////////

// Scoreboard types.
enum class scoreboard {
	TIME = 0,
	SCORE = 4
};

// Scoreboard screens of the main menu.
class scoreboard_state : public main_menu_state {
  public:
	// Creates a scoreboard state.
	scoreboard_state(std::shared_ptr<playerless_game> game, scoreboard scoreboard);

	tr::next_state tick() override;

  private:
	// Base substates of the scoreboard state.
	enum class substate_base {
		// In the scoreboard screen or entering it.
		IN_SCOREBOARD,
		// Switching a gamemode or pages.
		SWITCHING_PAGE,
		// Exiting to the scoreboard selection screen.
		EXITING
	};
	// Substates of the scoreboard state (substate_base + scoreboard).
	enum class substate {
	};

	// The current substate.
	substate m_substate;
	// The currently open page.
	int m_page;
	// The currently selected gamemode.
	std::vector<score_category>::iterator m_selected;
	// List of scores sorted by either score or time depending on the scoreboard.
	std::vector<score_entry> m_sorted_scores;
	// Holds the result of an asynchronously loaded new set of widgets.
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	// Combines substate components.
	friend substate operator|(const substate_base& l, const scoreboard& r);
	// Gets the base from a substate value.
	friend substate_base to_base(substate state);
	// Gets the scoreboard type from a substate value.
	friend scoreboard to_scoreboard(substate state);

	// Sets up the UI page switching animation.
	void set_up_page_switch_animation();
	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

////////////////////////////////////////////////////////////// REPLAYS STATE //////////////////////////////////////////////////////////////

// Replay screen of the main menu.
class replays_state : public main_menu_state {
  public:
	// Creates a replays state returning from a replay.
	replays_state();
	// Creates a replays state coming from the title screen.
	replays_state(std::shared_ptr<playerless_game> game);

  public:
	tr::next_state tick() override;

  private:
	// Substates of the replays state.
	enum class substate {
		// Fading in while returning from a replay game.
		RETURNING_FROM_REPLAY,
		// In the replays screen.
		IN_REPLAYS,
		// Switching pages.
		SWITCHING_PAGE,
		// Fading out and starting a replay.
		STARTING_REPLAY,
		// Exiting to the title screen.
		EXITING
	};

	// The current substate.
	substate m_substate;
	// The currently open page.
	int m_page;
	// List of available replays.
	replay_map m_replays;
	// Holds the result of an asynchronously loaded new set of widgets.
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	float fade_overlay_opacity() override;

	// Prepares the widgets for the next page.
	std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets();
	// Sets up the UI when entering.
	void set_up_ui();
	// Sets up the UI page switching animation.
	void set_up_page_switch_animation();
	// Sets up the UI exit animation.
	void set_up_exit_animation();

	friend struct replay_widget;
};

////////////////////////////////////////////////////////////// SETTINGS STATE /////////////////////////////////////////////////////////////

// Settings screen of the main menu.
class settings_state : public main_menu_state {
  public:
	// Creates a settings state when entering from the title screen.
	settings_state(std::shared_ptr<playerless_game> game);

	tr::next_state tick() override;

  private:
	// Substates of the credits state.
	enum class substate {
		// In the settings screen or entering it.
		IN_SETTINGS,
		// Exiting before entering the title screen.
		EXITING
	};

	// The current substate.
	substate m_substate;
	// The pending settings.
	settings m_pending;

	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

////////////////////////////////////////////////////////////// CREDITS STATE //////////////////////////////////////////////////////////////

// Credits screen of the main menu.
class credits_state : public main_menu_state {
  public:
	// Creates a credits state when entering from the title screen.
	credits_state(std::shared_ptr<playerless_game> game);

	tr::next_state tick() override;

  private:
	// Substates of the credits state.
	enum class substate {
		// In the credits screen or entering it.
		IN_CREDITS,
		// Exiting before entering the title screen.
		EXITING,
	};

	// The current substate.
	substate m_substate;
};

/////////////////////////////////////////////////////////////// GAME STATE ////////////////////////////////////////////////////////////////

// Whether a fade in should be done for the game state.
enum class fade_in : bool {
	NO,
	YES
};

// Ongoing game state.
class game_state : public state {
  public:
	// Creates a new game state.
	game_state(std::shared_ptr<game> game, game_type type, fade_in fade_in);

	tr::next_state handle_event(const tr::sys::event& event) override;
	tr::next_state tick() override;
	void draw() override;

  private:
	// Base substates of the game state.
	enum class substate_base {
		// Fading into the game state.
		FADING_IN,
		// The game is ongoing.
		ONGOING,
		// Game overed, but didnt switch to game_over_state yet.
		GAME_OVER,
		// Fading out and exiting to menu.
		EXITING
	};
	// Substates of the game state (substate_base + game_type).
	enum class substate {
	};

	// The current substate.
	substate m_substate;
	// Pointer to the game being played.
	std::shared_ptr<game> m_game;

	// Combines substate components.
	friend substate operator|(const substate_base& l, const game_type& r);
	// Gets the base from a substate value.
	friend substate_base to_base(substate state);
	// Gets the game type from a substate value.
	friend game_type to_type(substate state);

	// Calculates the opacity of the fade overlay.
	float fade_overlay_opacity() const;

	// Adds a visual of the cursor position of the player in the replay to the renderer.
	void add_replay_cursor_to_renderer(glm::vec2 pos) const;
};

// Asynchronously creates a game state.
template <class T, class... Ts>
std::future<tr::next_state> make_game_state_async(game_type type, fade_in fade_in, Ts&&... gargs)
	requires(std::constructible_from<T, Ts...>)
{
	return std::async(
		std::launch::async,
		[]<class... Us>(game_type type, ::fade_in fade_in, Us&&... gargs) {
			return (tr::next_state)std::make_unique<game_state>(std::make_shared<T>(std::forward<Us>(gargs)...), type, fade_in);
		},
		type, fade_in, std::forward<Ts>(gargs)...);
}

/////////////////////////////////////////////////////////////// PAUSE STATE ///////////////////////////////////////////////////////////////

// Whether a blur in should be done for a state.
enum class blur_in : bool {
	NO,
	YES
};

// Pause screen state.
class pause_state : public game_menu_state {
  public:
	// Creates a pause state.
	pause_state(std::shared_ptr<game> game, game_type type, glm::vec2 mouse_pos, blur_in blur_in);

	tr::next_state tick() override;

  private:
	// Base substates of the pause state.
	enum class substate_base {
		// Pausing the game.
		PAUSING,
		// In the pause screen.
		PAUSED,
		// Unpausing the game.
		UNPAUSING,
		// Entering the save score screen.
		SAVING,
		// Restarting the game.
		RESTARTING,
		// Quitting to the main menu.
		QUITTING
	};
	// Substates of the pause state (substate_base + game_type).
	enum class substate {
	};

	// The current substate.
	substate m_substate;
	// The mouse position to restore when unpausing.
	glm::vec2 m_start_mouse_pos;
	// The mouse position right before unpausing.
	glm::vec2 m_end_mouse_pos;

	// Combines substate components.
	friend substate operator|(const substate_base& l, const game_type& r);
	// Gets the base from a substate value.
	friend substate_base to_base(substate state);
	// Gets the game type from a substate value.
	friend game_type to_type(substate state);

	float fade_overlay_opacity() override;
	float saturation_factor() override;
	float blur_strength() override;

	// Sets up the full UI when entering.
	void set_up_full_ui();
	// Sets up the partial UI when entering.
	void set_up_limited_ui();
	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

///////////////////////////////////////////////////////////// GAME OVER STATE /////////////////////////////////////////////////////////////

// Game over screen state.
class game_over_state : public game_menu_state {
  public:
	// Creates a game over state.
	game_over_state(std::shared_ptr<game> game, blur_in blur_in);

	tr::next_state tick() override;

  private:
	// Substates of the game over state.
	enum class substate {
		// Entering the game over screen after game overing.
		BLURRING_IN,
		// In the game over screen.
		GAME_OVER,
		// Entering the save score screen.
		SAVING,
		// Restarting the game.
		RESTARTING,
		// Quitting to the title screen.
		QUITTING
	};

	// The current substate.
	substate m_substate;

	float fade_overlay_opacity() override;
	float saturation_factor() override;
	float blur_strength() override;

	// Creates a text callback for the "best time" widget.
	text_callback best_time_text_callback() const;
	// Creates a text callback for the "best score" widget.
	text_callback best_score_text_callback() const;

	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////////// SAVE SCORE STATE /////////////////////////////////////////////////////////////

// Flags for the save screen states.
enum class save_screen_flags : u8 {
	NONE = 0x0,
	GAME_OVER = 0x4,
	RESTARTING = 0x8,
	MASK = 0xC
};
DEFINE_BITMASK_OPERATORS(save_screen_flags);

// Score saving screen of the pause and game over menus.
class save_score_state : public game_menu_state {
  public:
	// Creates a save score state coming from the pause screen.
	save_score_state(std::shared_ptr<game> game, glm::vec2 mouse_pos, save_screen_flags flags);
	// Creates a save score state coming from the game over screen.
	save_score_state(std::shared_ptr<game> game, save_screen_flags flags);

	tr::next_state tick() override;

  private:
	// Base substates of the save score state.
	enum class substate_base {
		// In the save score screen.
		SAVING_SCORE = 0x0,
		// Transitioning to another screen.
		RETURNING_OR_ENTERING_SAVE_REPLAY = 0x1,
	};
	// Substates of the save score state (substate_base + save_screen_flags).
	enum class substate {
	};

	// The current substate.
	substate m_substate;
	// The mouse position to restore when unpausing.
	glm::vec2 m_start_mouse_pos;
	// The score to save.
	score_entry m_score;

	// Combines substate components.
	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	// Gets the base from a substate value.
	friend substate_base to_base(substate state);
	// Gets the save screen flags from a substate value.
	friend save_screen_flags to_flags(substate state);

	// Sets up the UI when entering.
	void set_up_ui();
	// Sets up the UI exit animation.
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////////// SAVE REPLAY STATE ////////////////////////////////////////////////////////////

// Replay saving screen of the pause and game over menus.
class save_replay_state : public game_menu_state {
  public:
	// Creates the save replay state.
	save_replay_state(std::shared_ptr<game> game, save_screen_flags flags);

	tr::next_state tick() override;

  private:
	// Base substates of the save replay state.
	enum class substate_base {
		// In the save replay screen.
		SAVING_REPLAY = 0x0,
		// Exiting to the title screen.
		EXITING = 0x1
	};
	// Substates of the save replay state (substate_base + save_screen_flags).
	enum class substate {
	};

	// The current substate.
	substate m_substate;
	// Header of the replay to save.
	replay_header m_replay;

	// Combines substate components.
	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	// Gets the base from a substate value.
	friend substate_base to_base(substate state);
	// Gets the save screen flags from a substate value.
	friend save_screen_flags to_flags(substate state);

	float fade_overlay_opacity() override;

	// Sets up the UI exit animation.
	void set_up_exit_animation();
};