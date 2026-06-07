///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// State hierarchy:                                                                                                                      //
// • state                                - Base state interface.                                                                        //
//     • main_menu_state                  - Base main menu state interface.                                                              //
//         • name_entry_state             - Name entry screen shown on first launch.                                                     //
//         • title_state                  - Title/main menu screen.                                                                      //
//         • start_game_state             - Gamemode selection screen.                                                                   //
//         • gamemode_designer_state      - Gamemode designer screen.                                                                    //
//         • ball_settings_editor_state   - Ball settings subscreen of the gamemode designer.                                            //
//         • player_settings_editor_state - Player settings subscreen of the gamemode designer.                                          //
//         • scoreboard_selection_state   - Scoreboard type selection screen.                                                            //
//         • scoreboard_state             - Scoreboard screen.                                                                           //
//         • replays_state                - Replay selection screen.                                                                     //
//         • settings_state               - Settings screen.                                                                             //
//         • credits_state                - Credits screen.                                                                              //
//     • game_menu_state                  - Base game menu state interface.                                                              //
//         • pause_state                  - Pause screen.                                                                                //
//         • game_over_state              - Game over screen.                                                                            //
//         • save_score_state             - Save score subscreen of the pause/game over screen.                                          //
//         • save_replay_state            - Save replay subscreen of the pause/game over screen.                                         //
//     • game_state                       - Game state (regular, test, or replay).                                                       //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "state/state_base.hpp"

using gamemode_widget_action_command = std::function<void(const gamemode_with_path&)>;
class gamemode_editor_state;
class gamemode_selector_state;

////////////////////////////////////////////////////////////// STATE MACHINE //////////////////////////////////////////////////////////////

// Current state singleton.
class current_state : public tr::state_machine {
  public:
	// Gets the current state instance.
	static current_state& instance();

	// Accesses the current state.
	state* operator->();

	// Handles an event and returns a signal.
	tr::sys::signal handle_event(const tr::sys::event& event);
	// Updates the state and returns a signal.
	tr::sys::signal tick();

  private:
	using state_machine::handle_event;
	using state_machine::tick;

	// Creates an initial state.
	current_state();
};

//////////////////////////////////////////////////////////// NAME ENTRY STATE /////////////////////////////////////////////////////////////

// Name entry screen of the main menu that appears on first launch.
class name_entry_state final : public main_menu_state {
  public:
	// Creates the name entry state.
	name_entry_state();

	// Updates the state.
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

	// The opacity of the fade overlay.
	float fade_overlay_opacity() override;

	// Function called upon exiting from the name entry screen.
	void on_exit();
};

/////////////////////////////////////////////////////////////// TITLE STATE ///////////////////////////////////////////////////////////////

// Title screen of the main menu.
class title_state final : public main_menu_state {
  public:
	// Creates a title state when starting.
	title_state();
	// Creates a title state when entering from the name entry state.
	title_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game);

	// Updates the state.
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

	// The opacity of the fade overlay.
	float fade_overlay_opacity() override;

	// Sets up the UI when entering.
	void set_up_ui();
	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the "start game" button is pressed.
	void on_start_game();
	// Function called when the "gamemode manager" button is pressed.
	void on_gamemode_manager();
	// Function called when the "scoreboards" button is pressed.
	void on_scoreboards();
	// Function called when the "replays" button is pressed.
	void on_replays();
	// Function called when the "settings" button is pressed.
	void on_settings();
	// Function called when the "credits" button is pressed.
	void on_credits();
	// Function called when the "exit" button is pressed.
	void on_exit();
};

//////////////////////////////////////////////////////////// START GAME STATE /////////////////////////////////////////////////////////////

// Start game screen of the main menu.
class start_game_state final : public main_menu_state {
  public:
	// Creates a start game state.
	start_game_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, savefile savefile);
	// Destroys the state.
	~start_game_state() override;

	// Signals whether the cursor should be drawn transparent.
	bool transparent_cursor() const override;
	// Updates the state.
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
	// Cached copy of the savefile.
	savefile m_savefile;
	// List of available gamemodes.
	std::vector<gamemode_with_path> m_gamemodes;
	// The currently selected gamemode.
	std::vector<gamemode_with_path>::iterator m_selected;
	// Holds the result of an asynchronously loaded new set of widgets.
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	// The opacity of the fade overlay.
	float fade_overlay_opacity() override;

	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the "previous gamemode" arrow is pressed.
	void on_previous_gamemode();
	// Function called when the "next gamemode" arrow is pressed.
	void on_next_gamemode();
	// Function called when the "start" button is pressed.
	void on_start();
	// Function called when the "exit" button is pressed.
	void on_exit();
};

////////////////////////////////////////////////////////// GAMEMODE MANAGER STATE /////////////////////////////////////////////////////////

// Gamemode manager screen of the main menu.
class gamemode_manager_state final : public main_menu_state {
  public:
	// Creates a gamemode manager state.
	gamemode_manager_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, animate_title animate_title);

	// Updates the state.
	tr::next_state tick() override;

  private:
	// Substates of the gamemode manager state.
	enum class substate {
		// In the gamemode manager screen.
		IN_GAMEMODE_MANAGER,
		// Exiting to either a subscreen or the title screen.
		EXITING,
	};

	// The current substate.
	substate m_substate;

	// Sets up the UI exit animation.
	void set_up_exit_animation(animate_title animate_title);

	// Function called when the "new gamemode" button is pressed.
	void on_enter_new_gamemode();
	// Function called when the "edit gamemode" button is pressed.
	void on_enter_edit_gamemode();
	// Function called when the "clone gamemode" button is pressed.
	void on_enter_clone_gamemode();
	// Function called when the "delete gamemode" button is pressed.
	void on_enter_delete_gamemode();
	// Function called upon exiting from the gamemode manager.
	void on_exit();
};

////////////////////////////////////////////////////////// GAMEMODE EDITOR STATE //////////////////////////////////////////////////////////

// Data specific to a new gamemode editor.
class new_gamemode_editor {
  public:
	// Constructs new gamemode editor data.
	new_gamemode_editor(savefile savefile);

	// Gets the text command used for the subtitle of the gamemode editor.
	localized_text subtitle_text(const localization& localization) const;
	// Function called by the gamemode editor state on save.
	void on_save(gamemode_editor_state& state);
	// Function called by the gamemode editor state on discard.
	void on_discard(gamemode_editor_state& state);

  private:
	// Cached copy of the savefile.
	savefile m_savefile{};
};
// Data specific to a cloned gamemode editor.
class cloned_gamemode_editor {
  public:
	// Gets the text command used for the subtitle of the gamemode editor.
	localized_text subtitle_text(const localization& localization) const;
	// Function called by the gamemode editor state on save.
	void on_save(gamemode_editor_state& state) const;
	// Function called by the gamemode editor state on discard.
	void on_discard(gamemode_editor_state& state) const;
};
// Data specific to an edited gamemode editor.
class edited_gamemode_editor {
  public:
	// Constructs edited gamemode editor data.
	edited_gamemode_editor(std::filesystem::path path);

	// Gets the text command used for the subtitle of the gamemode editor.
	localized_text subtitle_text(const localization& localization) const;
	// Function called by the gamemode editor state on save.
	void on_save(gamemode_editor_state& state) const;
	// Function called by the gamemode editor state on discard.
	void on_discard(gamemode_editor_state& state) const;

  private:
	// Path to the edited gamemode.
	std::filesystem::path m_path;
};
// Generic gamemode editor.
using gamemode_editor = std::variant<new_gamemode_editor, cloned_gamemode_editor, edited_gamemode_editor>;

// Gamemode editor screens of the main menu.
class gamemode_editor_state final : public main_menu_state {
  public:
	// Creates a gamemode editor state coming from a test game.
	gamemode_editor_state(std::shared_ptr<subsystems> subsystems, gamemode_editor data, gamemode gamemode);
	// Creates a gamemode editor state coming from another menu state.
	gamemode_editor_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, gamemode_editor data,
						  gamemode gamemode, animate_subtitle animate_subtitle);

	// Signals whether the cursor should be drawn transparent.
	bool transparent_cursor() const override;
	// Updates the state.
	tr::next_state tick() override;

  private:
	// Substates of the gamemode editor state.
	enum class substate {
		// Fading in and returning to the gamemode editor screen from a test game.
		RETURNING_FROM_TEST_GAME,
		// In the gamemode editor screen.
		IN_GAMEMODE_EDITOR,
		// Fading out and entering a test game.
		ENTERING_TEST_GAME,
		// Exiting to either a subscreen or the previous screen.
		EXITING,
	};

	// The current substate.
	substate m_substate;
	// Editor type.
	gamemode_editor m_type;
	// List of names of available songs.
	std::vector<std::string> m_available_songs;
	// The pending gamemode.
	gamemode m_pending;

	// The opacity of the fade overlay.
	float fade_overlay_opacity() override;

	// Sets up the UI when entering.
	void set_up_ui(animate_title animate_title, animate_subtitle animate_subtitle);
	// Sets up the UI exit animation.
	void set_up_exit_animation(animate_title animate_title, animate_subtitle animate_subtitle);

	// Function called when the "player settings" button is pressed.
	void on_enter_player_settings();
	// Function called when the "ball settings" button is pressed.
	void on_enter_ball_settings();
	// Function called when the song button is pressed.
	void on_change_song();
	// Function called when the "test" button is pressed.
	void on_test();
	// Function called when the "save" button is pressed.
	void on_save();
	// Function called when the "discard" button is pressed.
	void on_discard();

	friend class new_gamemode_editor;
	friend class edited_gamemode_editor;
	friend class cloned_gamemode_editor;
};

//////////////////////////////////////////////////////// BALL SETTINGS EDITOR STATE ///////////////////////////////////////////////////////

// Ball settings editor screen of the gamemode designer menu.
class ball_settings_editor_state final : public main_menu_state {
  public:
	// Creates a ball settings editor state.
	ball_settings_editor_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, gamemode_editor data,
							   gamemode gamemode);

	// Updates the state.
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
	// Type-dependent editor data.
	gamemode_editor m_data;
	// The pending gamemode.
	gamemode m_pending;

	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the "exit" button is pressed.
	void on_exit();
};

/////////////////////////////////////////////////////// PLAYER SETTINGS EDITOR STATE //////////////////////////////////////////////////////

// Player settings editor screen of the gamemode designer menu.
class player_settings_editor_state final : public main_menu_state {
  public:
	// Creates a player settings editor state.
	player_settings_editor_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, gamemode_editor data,
								 gamemode gamemode);

	// Updates the state.
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
	// Type-dependent editor data.
	gamemode_editor m_data;
	// The pending gamemode.
	gamemode m_pending;

	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the "spawn life fragments" setting is toggled.
	void on_toggle_life_fragments();
	// Function called when the "exit" button is pressed.
	void on_exit();
};

///////////////////////////////////////////////////////// GAMEMODE SELECTOR STATE /////////////////////////////////////////////////////////

// Selector for cloning gamemodes.
class clone_gamemode_selector {
  public:
	// Creates a clone selector.
	clone_gamemode_selector(tr::static_string<20 * 4> player_name);

	// Filters gamemodes such that only clonable gamemodes remain.
	void filter_gamemodes(std::vector<gamemode_with_path>& gamemodes) const;
	// Gets the text command used for the subtitle of the gamemode selector.
	localized_text subtitle_text(const localization& localization) const;
	// Function executed when a gamemode is selected.
	void on_gamemode_selected(gamemode_selector_state& state, const gamemode_with_path& gp) const;

  private:
	// Cached copy of the player's name.
	tr::static_string<20 * 4> m_player_name;
};
// Selector for editing gamemodes.
class edit_gamemode_selector {
  public:
	// Creates an editor selector.
	edit_gamemode_selector(tr::static_string<20 * 4> player_name);

	// Filters gamemodes such that only editable gamemodes remain.
	void filter_gamemodes(std::vector<gamemode_with_path>& gamemodes) const;
	// Gets the text command used for the subtitle of the gamemode selector.
	localized_text subtitle_text(const localization& localization) const;
	// Function executed when a gamemode is selected.
	void on_gamemode_selected(gamemode_selector_state& state, const gamemode_with_path& gp) const;

  private:
	// Cached copy of the player's name.
	tr::static_string<20 * 4> m_player_name;
};
// Selector for deleting gamemodes.
class delete_gamemodes_selector {
  public:
	// Filters gamemodes such that only deletable gamemodes remain.
	void filter_gamemodes(std::vector<gamemode_with_path>& gamemodes) const;
	// Gets the text command used for the subtitle of the gamemode selector.
	localized_text subtitle_text(const localization& localization) const;
	// Function executed when a gamemode is selected.
	void on_gamemode_selected(gamemode_selector_state& state, const gamemode_with_path& gp) const;
};
// Generic gamemode selector.
using gamemode_selector = std::variant<clone_gamemode_selector, edit_gamemode_selector, delete_gamemodes_selector>;

// Gamemode selector screens of the main menu.
class gamemode_selector_state final : public main_menu_state {
  public:
	// Creates a gamemode selector state.
	gamemode_selector_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, gamemode_selector selector,
							animate_subtitle move_subtitle);

	// Updates the state.
	tr::next_state tick() override;

  private:
	// Substates of the gamemode selector state.
	enum class substate {
		// In the gamemode selector screen or entering it.
		IN_GAMEMODE_SELECTOR,
		// Switching pages.
		SWITCHING_PAGE,
		// Exiting to the gamemode manager screen or a gamemode editor screen.
		EXITING
	};

	// The current substate.
	substate m_substate;
	// Concete selector type.
	gamemode_selector m_selector;
	// List of available gamemodes.
	std::vector<gamemode_with_path> m_gamemodes;
	// The currently open page.
	int m_page;
	// Holds the result of an asynchronously loaded new set of widgets.
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	// Prepares the widgets for the next page.
	std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets();
	// Sets up the UI page switching animation.
	void set_up_page_switch_animation();
	// Sets up the UI exit animation.
	void set_up_exit_animation(animate_subtitle move_subtitle);

	// Function called when a gamemode is selected.
	void on_gamemode_selected(const gamemode_with_path& gp);
	// Function called when the "exit" button is pressed.
	void on_exit();
	// Function called when the page is decremented.
	void on_page_decrement();
	// Function called when the page is incremented.
	void on_page_increment();

	friend class clone_gamemode_selector;
	friend class edit_gamemode_selector;
	friend class delete_gamemodes_selector;
};

//////////////////////////////////////////////////////// SCOREBOARD SELECTION STATE ///////////////////////////////////////////////////////

// Scoreboard selection screen of the main menu.
class scoreboard_selection_state final : public main_menu_state {
  public:
	// Creates a scoreboard selection state.
	scoreboard_selection_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, savefile savefile,
							   animate_title animate_title);

	// Updates the state.
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
	// Cached copy of the savefile.
	savefile m_savefile;

	// Sets up the UI exit animation.
	void set_up_exit_animation(animate_title animate_title);

	// Function called when the "view times" button is pressed.
	void on_view_times();
	// Function called when the "view scores" button is pressed.
	void on_view_scores();
	// Function called when the "exit" button is pressed.
	void on_exit();
};

//////////////////////////////////////////////////////////// SCOREBOARDS STATE ////////////////////////////////////////////////////////////

// Scoreboard types.
enum class scoreboard {
	TIME = 0,
	SCORE = 4
};

// Scoreboard screens of the main menu.
class scoreboard_state final : public main_menu_state {
  public:
	// Creates a scoreboard state.
	scoreboard_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game, savefile savefile,
					 scoreboard scoreboard);

	// Updates the state.
	tr::next_state tick() override;

  private:
	// Substates of the scoreboard state.
	enum class substate {
		// In the scoreboard screen or entering it.
		IN_SCOREBOARD,
		// Switching a gamemode or pages.
		SWITCHING_PAGE,
		// Exiting to the scoreboard selection screen.
		EXITING
	};

	// The current substate.
	substate m_substate;
	// The scoreboard type.
	scoreboard m_scoreboard;
	// The currently open page.
	int m_page;
	// Cached copy of the savefile.
	savefile m_savefile;
	// The currently selected gamemode.
	std::vector<score_category>::const_iterator m_selected;
	// List of scores sorted by either score or time depending on the scoreboard.
	std::vector<score_entry> m_sorted_scores;
	// Holds the result of an asynchronously loaded new set of widgets.
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	// Sets up the UI page switching animation.
	void set_up_page_switch_animation();
	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the "exit" button is pressed.
	void on_exit();
	// Function called when the gamemode is decremented.
	void on_gamemode_decrement();
	// Function called when the gamemode is incremented.
	void on_gamemode_increment();
	// Function called when the page is decremented.
	void on_page_decrement();
	// Function called when the page is incremented.
	void on_page_increment();
};

////////////////////////////////////////////////////////////// REPLAYS STATE //////////////////////////////////////////////////////////////

// Replay screen of the main menu.
class replays_state final : public main_menu_state {
  public:
	// Creates a replays state returning from a replay.
	replays_state(std::shared_ptr<subsystems> subsystems);
	// Creates a replays state coming from the title screen.
	replays_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game);

	// Signals whether the cursor should be drawn transparent.
	bool transparent_cursor() const override;
	// Updates the state.
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

	// The opacity of the fade overlay.
	float fade_overlay_opacity() override;

	// Prepares the widgets for the next page.
	std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets();
	// Sets up the UI when entering.
	void set_up_ui();
	// Sets up the UI page switching animation.
	void set_up_page_switch_animation();
	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the "exit" button is pressed.
	void on_exit();
	// Function called when the page is decremented.
	void on_page_decrement();
	// Function called when the page is incremented.
	void on_page_increment();

	friend class replay_widget;
};

////////////////////////////////////////////////////////////// SETTINGS STATE /////////////////////////////////////////////////////////////

// Settings screen of the main menu.
class settings_state final : public main_menu_state {
  public:
	// Creates a settings state when entering from the title screen.
	settings_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game);

	// Updates the state.
	tr::next_state tick() override;

  private:
	// Substates of the credits state.
	enum class substate {
		// In the settings screen or entering it.
		IN_SETTINGS,
		// Exiting before entering the title screen.
		EXITING
	};

	// Current substate.
	substate m_substate;
	// Pending settings.
	settings m_pending;
	// Available player skins.
	std::vector<std::string> m_player_skins;

	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the display mode is changed.
	void on_change_display_mode();
	// Function called when the player skin is changed.
	void on_change_player_skin();
	// Function called when the language is changed.
	void on_change_language();
	// Function called when the "revert" button is pressed.
	void on_revert();
	// Function called when the "apply" button is pressed.
	void on_apply();
	// Function called when the "exit" button is pressed.
	void on_exit();
};

////////////////////////////////////////////////////////////// CREDITS STATE //////////////////////////////////////////////////////////////

// Credits screen of the main menu.
class credits_state final : public main_menu_state {
  public:
	// Creates a credits state when entering from the title screen.
	credits_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<playerless_game> game);

	// Updates the state.
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

	// Function called upon exiting the credits menu.
	void on_exit();
};

/////////////////////////////////////////////////////////////// GAME STATE ////////////////////////////////////////////////////////////////

// Data specific to a regular game state.
struct regular_game_data {};
// Data specific to a replay game state.
struct replay_game_data {};
// Data specific to a test game state.
struct test_game_data {
	// Properties of the gamemode editor.
	gamemode_editor editor_data;
};
// Game state data.
using game_state_data = std::variant<regular_game_data, replay_game_data, test_game_data>;

// Whether a fade in should be done for the game state.
enum class fade_in : bool {
	NO,
	YES
};

// Ongoing game state.
class game_state final : public state {
  public:
	// Creates a new game state.
	game_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<game> game, game_state_data data, fade_in fade_in);

	// Signals whether the cursor should be drawn transparent.
	bool transparent_cursor() const override;
	// Handles an event.
	tr::next_state handle_event(const tr::sys::event& event) override;
	// Updates the state.
	tr::next_state tick() override;
	// Draws the state.
	void draw() override;

  private:
	// Substates of the game state.
	enum class substate {
		// Fading into the game state.
		FADING_IN,
		// The game is ongoing.
		ONGOING,
		// Game overed, but didnt switch to game_over_state yet.
		GAME_OVER,
		// Fading out and exiting to menu.
		EXITING
	};

	// The current substate.
	substate m_substate;
	// Type-specific state data.
	game_state_data m_data;
	// The speed multiplier of the playing song.
	float m_song_speed;
	// Pointer to the game being played.
	std::shared_ptr<game> m_game;

	// Calculates the opacity of the fade overlay.
	float fade_overlay_opacity() const;

	// Sets the song of the playing speed if needed.
	void set_song_speed_if_needed(float speed);

	// Adds a visual of the cursor position of the player in the replay to the renderer.
	void add_replay_cursor_to_renderer(glm::vec2 pos) const;
};

// Asynchronously creates a game state.
template <class T, class... Ts>
std::future<tr::next_state> make_game_state_async(std::shared_ptr<state::subsystems> subsystems, game_state_data data, Ts... gargs)
	requires(std::constructible_from<T, Ts...>)
{
	constexpr auto ctor{[](std::shared_ptr<state::subsystems> subsystems, game_state_data data, auto... gargs) {
		return (tr::next_state)std::make_unique<game_state>(std::move(subsystems), std::make_shared<T>(std::move(gargs)...), data,
															fade_in::YES);
	}};
	return std::async(std::launch::async, ctor, std::move(subsystems), std::move(data), std::move(gargs)...);
}

/////////////////////////////////////////////////////////////// PAUSE STATE ///////////////////////////////////////////////////////////////

// Whether a blur in should be done for a state.
enum class blur_in : bool {
	NO,
	YES
};

// Pause screen state.
class pause_state final : public game_menu_state {
  public:
	// Creates a test game pause state.
	pause_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<game> game, savefile savefile, game_state_data data,
				glm::vec2 mouse_pos, blur_in blur_in);

	// Signals whether the cursor should be drawn transparent.
	bool transparent_cursor() const override;
	// Updates the state.
	tr::next_state tick() override;

  private:
	// Substates of the pause state.
	enum class substate {
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

	// The current substate.
	substate m_substate;
	// Type-specific state data.
	game_state_data m_data;
	// The mouse position to restore when unpausing.
	glm::vec2 m_start_mouse_pos;
	// The mouse position right before unpausing.
	glm::vec2 m_end_mouse_pos;

	// The opacity of the fade overlay.
	float fade_overlay_opacity() override;
	// The saturation of the background game.
	float saturation_factor() override;
	// The strength of the background blur.
	float blur_strength() override;

	// Sets up the full UI when entering.
	void set_up_full_ui();
	// Sets up the partial UI when entering.
	void set_up_limited_ui();
	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the "unpause" button is pressed.
	void on_unpause();
	// Function called when the "save and restart" button is pressed.
	void on_save_and_restart();
	// Function called when the "restart" button is pressed.
	void on_restart();
	// Function called when the "save and quit" button is pressed.
	void on_save_and_quit();
	// Function called when the "quit" button is pressed.
	void on_quit();
};

///////////////////////////////////////////////////////////// GAME OVER STATE /////////////////////////////////////////////////////////////

// Game over screen state.
class game_over_state final : public game_menu_state {
  public:
	// Creates a game over state.
	game_over_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<game> game, savefile savefile, blur_in blur_in);

	// Signals whether the cursor should be drawn transparent.
	bool transparent_cursor() const override;
	// Updates the state.
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

	// The opacity of the fade overlay.
	float fade_overlay_opacity() override;
	// The saturation of the background game.
	float saturation_factor() override;
	// The strength of the background blur.
	float blur_strength() override;

	// Creates a text command for the "best time" widget.
	text_command best_time_text() const;
	// Creates a text command for the "best score" widget.
	text_command best_score_text() const;

	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called upon pressing "save and restart".
	void on_save_and_restart();
	// Function called upon pressing "restart".
	void on_restart();
	// Function called upon pressing "save and exit".
	void on_save_and_exit();
	// Function called upon pressing "exit".
	void on_exit();
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
class save_score_state final : public game_menu_state {
  public:
	// Creates a save score state coming from the pause screen.
	save_score_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<game> game, savefile savefile, glm::vec2 mouse_pos,
					 save_screen_flags flags);
	// Creates a save score state coming from the game over screen.
	save_score_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<game> game, savefile savefile, save_screen_flags flags);

	// Updates the state.
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

	// Function called when the "save" button is pressed.
	void on_save();
	// Function called when the "cancel" button is pressed.
	void on_cancel();
};

//////////////////////////////////////////////////////////// SAVE REPLAY STATE ////////////////////////////////////////////////////////////

// Replay saving screen of the pause and game over menus.
class save_replay_state final : public game_menu_state {
  public:
	// Creates the save replay state.
	save_replay_state(std::shared_ptr<subsystems> subsystems, std::shared_ptr<game> game, savefile savefile, save_screen_flags flags);

	// Signals whether the cursor should be drawn transparent.
	bool transparent_cursor() const override;
	// Updates the state.
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

	// The opacity of the fade overlay.
	float fade_overlay_opacity() override;

	// Sets up the UI exit animation.
	void set_up_exit_animation();

	// Function called when the "save" button is pressed.
	void on_save();
	// Function called when the "discard" button is pressed.
	void on_discard();
};