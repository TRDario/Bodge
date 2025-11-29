#pragma once
#include "state_base.hpp" // IWYU pragma: export
#include <future>

//////////////////////////////////////////////////////////// NAME ENTRY STATE /////////////////////////////////////////////////////////////

class name_entry_state : public main_menu_state {
  public:
	name_entry_state();

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		FADING_IN,
		IN_NAME_ENTRY,
		ENTERING_TITLE
	};

	substate m_substate;

	float fade_overlay_opacity() override;

	static bool input_interactible();
	static bool confirm_interactible();
	static void on_confirm();
};

/////////////////////////////////////////////////////////////// TITLE STATE ///////////////////////////////////////////////////////////////

class title_state : public main_menu_state {
  public:
	title_state();
	title_state(std::shared_ptr<playerless_game> game);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		ENTERING_GAME,
		IN_TITLE,
		ENTERING_SUBMENU,
		EXITING_GAME
	};

	substate m_substate;

	float fade_overlay_opacity() override;

	void set_up_exit_animation();

	static bool interactible();
	static void on_start_game();
	static void on_gamemode_designer();
	static void on_scoreboards();
	static void on_replays();
	static void on_settings();
	static void on_credits();
	static void on_exit();
	static constexpr std::array<void (*)(), 7> ACTION_CALLBACKS{on_start_game, on_gamemode_designer, on_scoreboards, on_replays,
																on_settings,   on_credits,           on_exit};
};

//////////////////////////////////////////////////////////// START GAME STATE /////////////////////////////////////////////////////////////

class start_game_state : public main_menu_state {
  public:
	start_game_state(std::shared_ptr<playerless_game> game);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		ENTERING_START_GAME,
		IN_START_GAME,
		SWITCHING_GAMEMODE,
		ENTERING_TITLE,
		ENTERING_GAME
	};

	substate m_substate;
	std::vector<gamemode> m_gamemodes;
	std::vector<gamemode>::iterator m_selected;
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	float fade_overlay_opacity() override;

	void set_up_exit_animation();

	static bool interactible();
	static bool arrow_interactible();
	static void on_prev();
	static void on_next();
	static void on_start();
	static void on_exit();
};

///////////////////////////////////////////////////////// GAMEMODE DESIGNER STATE /////////////////////////////////////////////////////////

class gamemode_designer_state : public main_menu_state {
  public:
	gamemode_designer_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode, bool returning_from_subscreen);
	gamemode_designer_state(const gamemode& gamemode);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		RETURNING_FROM_TEST_GAME,
		RETURNING_FROM_SUBSCREEN,
		IN_GAMEMODE_DESIGNER,
		ENTERING_TEST_GAME,
		ENTERING_SUBMENU_OR_TITLE,
	};

	substate m_substate;
	gamemode m_pending;
	std::vector<std::string> m_available_songs;

	float fade_overlay_opacity() override;

	void set_up_subscreen_animation();
	void set_up_exit_animation();

	static std::string song_c_text();
	static bool status_callback();
	static bool save_interactible();
	static constexpr std::array<bool (*)(), 3> BOTTOM_STATUS_CALLBACKS{status_callback, save_interactible, status_callback};
	static void on_name();
	static void on_description();
	static void on_ball_settings();
	static void on_player_settings();
	static void on_song_c();
	static void on_test();
	static void on_save();
	static void on_discard();
	static constexpr std::array<void (*)(), 3> BOTTOM_ACTION_CALLBACKS{on_test, on_save, on_discard};
};

//////////////////////////////////////////////////////// BALL SETTINGS EDITOR STATE ///////////////////////////////////////////////////////

class ball_settings_editor_state : public main_menu_state {
  public:
	ball_settings_editor_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		IN_EDITOR,
		EXITING
	};

	substate m_substate;
	gamemode m_pending;

	void set_up_exit_animation();

	static bool interactible();
	static bool starting_count_i_interactible();
	static bool max_count_d_interactible();
	static void on_starting_count_i();
	static void on_max_count_d();
	static void on_exit();
	static u8 starting_count_validation_callback(int value);
	static u8 max_count_validation_callback(int value);
};

/////////////////////////////////////////////////////// PLAYER SETTINGS EDITOR STATE //////////////////////////////////////////////////////

class player_settings_editor_state : public main_menu_state {
  public:
	player_settings_editor_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		IN_EDITOR,
		EXITING
	};

	substate m_substate;
	gamemode m_pending;

	void set_up_exit_animation();

	static std::string spawn_life_fragments_c_text();
	static bool interactible();
	static bool fragment_spawn_interval_interactible();
	static void on_spawn_life_fragments_c();
	static void on_exit();
};

//

class scoreboard_selection_state : public main_menu_state {
  public:
	scoreboard_selection_state(std::shared_ptr<playerless_game> game, bool returning_from_submenu);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		RETURNING_FROM_SUBMENU,
		IN_SCOREBOARD_SELECTION,
		ENTERING_SUBMENU_OR_TITLE,
	};

	substate m_substate;

	void set_up_subscreen_animation();
	void set_up_exit_animation();

	static bool interactible();
	static void on_view_times();
	static void on_view_scores();
	static void on_exit();
};

//////////////////////////////////////////////////////////// SCOREBOARDS STATE ////////////////////////////////////////////////////////////

enum class scoreboard {
	TIME = 0,
	SCORE = 4
};

class scoreboard_state : public main_menu_state {
  public:
	scoreboard_state(std::shared_ptr<playerless_game> game, scoreboard scoreboard);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate_base {
		IN_SCOREBOARD,
		SWITCHING_PAGE,
		EXITING_TO_SCOREBOARD_SELECTION
	};
	enum class substate {
	}; // substate_base + scoreboard.

	substate m_substate;
	int m_page;
	std::vector<score_category>::iterator m_selected;
	std::vector<score_entry> m_sorted_scores;
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	friend substate operator|(const substate_base& l, const scoreboard& r);
	friend substate_base to_base(substate state);
	friend scoreboard to_scoreboard(substate state);

	void set_up_page_switch_animation();
	void set_up_exit_animation();

	static std::string gamemode_c_tooltip();
	static std::string gamemode_c_text();
	static std::string page_c_text();
	static bool interactible();
	static bool gamemode_di_interactible();
	static bool page_d_interactible();
	static bool page_i_interactible();
	static void on_gamemode_d();
	static void on_gamemode_i();
	static void on_page_d();
	static void on_page_i();
	static void on_exit();
};

////////////////////////////////////////////////////////////// REPLAYS STATE //////////////////////////////////////////////////////////////

class replays_state : public main_menu_state {
  public:
	replays_state();
	replays_state(std::shared_ptr<playerless_game> game);

  public:
	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		RETURNING_FROM_REPLAY,
		IN_REPLAYS,
		SWITCHING_PAGE,
		STARTING_REPLAY,
		ENTERING_TITLE
	};

	substate m_substate;
	u16 m_page;
	std::map<std::string, replay_header> m_replays;
	std::map<std::string, replay_header>::const_iterator m_selected;
	std::future<std::unordered_map<tag, std::unique_ptr<widget>>> m_next_widgets;

	float fade_overlay_opacity() override;
	std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets();

	void set_up_page_switch_animation();
	void set_up_exit_animation();

	static std::string page_c_text();
	static bool interactible();
	static bool page_d_interactible();
	static bool page_i_interactible();
	static void on_replay(std::map<std::string, replay_header>::const_iterator it);
	static void on_page_d();
	static void on_page_i();
	static void on_exit();
};

////////////////////////////////////////////////////////////// SETTINGS STATE /////////////////////////////////////////////////////////////

class settings_state : public main_menu_state {
  public:
	settings_state(std::shared_ptr<playerless_game> game);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		IN_SETTINGS,
		ENTERING_TITLE
	};

	substate m_substate;
	settings m_pending;

	void set_up_exit_animation();

	static std::string display_mode_c_text();
	static std::string vsync_c_text();
	static std::string msaa_c_text();
	static std::string language_c_text();
	static bool interactible();
	static bool window_size_dc_interactible();
	static bool window_size_i_interactible();
	static bool msaa_d_interactible();
	static bool msaa_i_interactible();
	static bool revert_apply_interactible();
	static bool exit_interactible();
	static constexpr std::array<bool (*)(), 3> BOTTOM_STATUS_CALLBACKS{revert_apply_interactible, revert_apply_interactible,
																	   exit_interactible};
	static bool language_c_interactible();
	static void on_display_mode_c();
	static void on_window_size_i();
	static void on_vsync_c();
	static void on_msaa_d();
	static void on_msaa_i();
	static void on_primary_hue_d();
	static void on_primary_hue_i();
	static void on_secondary_hue_d();
	static void on_secondary_hue_i();
	static void on_language_c();
	static void on_revert();
	static void on_apply();
	static void on_exit();
	static constexpr std::array<void (*)(), 3> BOTTOM_ACTION_CALLBACKS{on_revert, on_apply, on_exit};
};

////////////////////////////////////////////////////////////// CREDITS STATE //////////////////////////////////////////////////////////////

class credits_state : public main_menu_state {
  public:
	credits_state(std::shared_ptr<playerless_game> game);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		IN_CREDITS,
		ENTERING_TITLE,
	};

	substate m_substate;

	static bool interactible();
	static void on_exit();
};

/////////////////////////////////////////////////////////////// GAME STATE ////////////////////////////////////////////////////////////////

class game_state : public state {
  public:
	game_state(std::shared_ptr<game> game, game_type type, bool fade_in);

	void set_up_ui();
	next_state handle_event(const tr::sys::event& event);
	next_state tick();
	void draw();

  private:
	enum class substate_base {
		FADING_IN,
		ONGOING,
		GAME_OVER,
		EXITING
	};
	enum class substate {
	}; // substate_base + game_type.

	substate m_substate;
	std::shared_ptr<game> m_game;

	friend substate operator|(const substate_base& l, const game_type& r);
	friend substate_base to_base(substate state);
	friend game_type to_type(substate state);

	float fade_overlay_opacity() const;
	void add_replay_cursor_to_renderer(glm::vec2 pos) const;
};

/////////////////////////////////////////////////////////////// PAUSE STATE ///////////////////////////////////////////////////////////////

class pause_state : public game_menu_state {
  public:
	pause_state(std::shared_ptr<game> game, game_type type, glm::vec2 mouse_pos, bool blur_in);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate_base {
		PAUSING,
		PAUSED,
		UNPAUSING,
		SAVING,
		RESTARTING,
		QUITTING,
	};
	enum class substate {
	}; // substate_base + game_type

	substate m_substate;
	glm::vec2 m_start_mouse_pos;
	glm::vec2 m_end_mouse_pos;

	friend substate operator|(const substate_base& l, const game_type& r);
	friend substate_base to_base(substate state);
	friend game_type to_type(substate state);

	float fade_overlay_opacity() override;
	float saturation_factor() override;
	float blur_strength() override;

	void set_up_full_ui();
	void set_up_limited_ui();
	void set_up_exit_animation();

	static bool interactible();
	static bool unpause_interactible();
	static void on_unpause();
	static void on_save_and_restart();
	static void on_restart();
	static void on_save_and_quit();
	static void on_quit();
	static constexpr std::array<void (*)(), 5> FULL_ACTION_CALLBACKS{on_unpause, on_save_and_restart, on_restart, on_save_and_quit,
																	 on_quit};
	static constexpr std::array<void (*)(), 3> LIMITED_ACTION_CALLBACKS{on_unpause, on_restart, on_quit};
};

///////////////////////////////////////////////////////////// GAME OVER STATE /////////////////////////////////////////////////////////////

class game_over_state : public game_menu_state {
  public:
	game_over_state(std::shared_ptr<game> game, bool blur_in);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate {
		BLURRING_IN,
		GAME_OVER,
		SAVING,
		RESTARTING,
		QUITTING
	};

	substate m_substate;

	float fade_overlay_opacity() override;
	float saturation_factor() override;
	float blur_strength() override;

	void set_up_exit_animation();

	static bool interactible();
	static void on_save_and_restart();
	static void on_restart();
	static void on_save_and_quit();
	static void on_quit();
	static constexpr std::array<void (*)(), 4> ACTION_CALLBACKS{on_save_and_restart, on_restart, on_save_and_quit, on_quit};
};

//////////////////////////////////////////////////////////// SAVE SCORE STATE /////////////////////////////////////////////////////////////

enum class save_screen_flags : u8 {
	NONE = 0x0,
	GAME_OVER = 0x4,
	RESTARTING = 0x8,
	MASK = 0xC
};
DEFINE_BITMASK_OPERATORS(save_screen_flags);

class save_score_state : public game_menu_state {
  public:
	// Used when coming from the pause screen.
	save_score_state(std::shared_ptr<game> game, glm::vec2 mouse_pos, save_screen_flags flags);
	// Used when coming from the game over screen.
	save_score_state(std::shared_ptr<game> game, save_screen_flags flags);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate_base {
		SAVING_SCORE = 0x0,
		RETURNING_OR_ENTERING_SAVE_REPLAY = 0x1,
	};
	enum class substate {
	}; // substate_base + save_screen_flags

	substate m_substate;
	glm::vec2 m_start_mouse_pos;
	score_entry m_score;

	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	friend substate_base to_base(substate state);
	friend save_screen_flags to_flags(substate state);

	void set_up_exit_animation();

	static bool interactible();
	static void on_save();
	static void on_cancel();
};

//////////////////////////////////////////////////////////// SAVE REPLAY STATE ////////////////////////////////////////////////////////////

class save_replay_state : public game_menu_state {
  public:
	save_replay_state(std::shared_ptr<game> game, save_screen_flags flags);

	void set_up_ui();
	next_state tick();

  private:
	enum class substate_base {
		SAVING_REPLAY = 0x0,
		EXITING = 0x1
	};
	enum class substate {
	}; // substate_base + save_screen_flags

	substate m_substate;
	replay_header m_replay;

	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	friend substate_base to_base(substate state);
	friend save_screen_flags to_flags(substate state);

	float fade_overlay_opacity() override;

	void set_up_exit_animation();

	static bool interactible();
	static bool save_interactible();
	static void on_name();
	static void on_save();
	static void on_discard();
};

////////////////////////////////////////////////////////////// STATE MACHINE //////////////////////////////////////////////////////////////

using state_machine =
	tr::state_machine<name_entry_state, title_state, start_game_state, gamemode_designer_state, ball_settings_editor_state,
					  player_settings_editor_state, scoreboard_selection_state, scoreboard_state, replays_state, settings_state,
					  credits_state, game_state, pause_state, game_over_state, save_score_state, save_replay_state>;

inline state_machine g_state_machine;

void set_main_menu_state();

/////////////////////////////////////////////////////////////// NEXT STATE ////////////////////////////////////////////////////////////////

inline std::future<next_state> g_next_state;

template <class T, class... Ts>
void prepare_next_state(Ts&&... args)
	requires(std::constructible_from<T, Ts...>)
{
	g_next_state = std::async(
		std::launch::async, []<class... Us>(Us&&... args) { return next_state{std::in_place_type_t<T>{}, std::forward<Us>(args)...}; },
		std::forward<Ts>(args)...);
}

template <class T, class... Ts>
void prepare_next_game_state(game_type type, bool fade_in, Ts&&... gargs)
	requires(std::constructible_from<T, Ts...>)
{
	g_next_state = std::async(
		std::launch::async,
		[]<class... Us>(game_type type, bool fade_in, Us&&... gargs) {
			return next_state{std::in_place_type_t<game_state>{}, std::make_shared<T>(std::forward<Us>(gargs)...), type, fade_in};
		},
		type, fade_in, std::forward<Ts>(gargs)...);
}