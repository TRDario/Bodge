#pragma once
#include "state_base.hpp"

//////////////////////////////////////////////////////////// NAME ENTRY STATE /////////////////////////////////////////////////////////////

class name_entry_state : public main_menu_state {
  public:
	name_entry_state();

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		FADING_IN,
		IN_NAME_ENTRY,
		ENTERING_TITLE
	};

	substate m_substate;

	float fade_overlay_opacity() override;
};

/////////////////////////////////////////////////////////////// TITLE STATE ///////////////////////////////////////////////////////////////

class title_state : public main_menu_state {
  public:
	title_state();
	title_state(std::unique_ptr<playerless_game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		ENTERING_GAME,
		IN_TITLE,
		ENTERING_START_GAME,
		ENTERING_GAMEMODE_DESIGNER,
		ENTERING_REPLAYS,
		ENTERING_SCOREBOARDS,
		ENTERING_SETTINGS,
		ENTERING_CREDITS,
		EXITING_GAME
	};

	substate m_substate;

	float fade_overlay_opacity() override;
	void set_up_ui();
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////////// START GAME STATE /////////////////////////////////////////////////////////////

class start_game_state : public main_menu_state {
  public:
	start_game_state(std::unique_ptr<playerless_game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

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

	float fade_overlay_opacity() override;
	void set_up_exit_animation();
};

///////////////////////////////////////////////////////// GAMEMODE DESIGNER STATE /////////////////////////////////////////////////////////

class gamemode_designer_state : public main_menu_state {
  public:
	gamemode_designer_state(std::unique_ptr<playerless_game>&& game, const gamemode& gamemode, bool returning_from_subscreen);
	gamemode_designer_state(const gamemode& gamemode);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		RETURNING_FROM_TEST_GAME,
		IN_GAMEMODE_DESIGNER,
		ENTERING_TEST_GAME,
		ENTERING_BALL_SETTINGS_EDITOR,
		ENTERING_PLAYER_SETTINGS_EDITOR,
		ENTERING_TITLE
	};

	substate m_substate;
	gamemode m_pending;
	std::vector<std::string> m_available_songs;

	float fade_overlay_opacity() override;
	void set_up_ui(bool returning_from_subscreen);
	void set_up_subscreen_animation();
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////// BALL SETTINGS EDITOR STATE ///////////////////////////////////////////////////////

class ball_settings_editor_state : public main_menu_state {
  public:
	ball_settings_editor_state(std::unique_ptr<playerless_game>&& game, const gamemode& gamemode);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_EDITOR,
		EXITING
	};

	substate m_substate;
	gamemode m_pending;

	void set_up_exit_animation();
};

/////////////////////////////////////////////////////// PLAYER SETTINGS EDITOR STATE //////////////////////////////////////////////////////

class player_settings_editor_state : public main_menu_state {
  public:
	player_settings_editor_state(std::unique_ptr<playerless_game>&& game, const gamemode& gamemode);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_EDITOR,
		EXITING
	};

	substate m_substate;
	gamemode m_pending;

	void set_up_exit_animation();
};

//

class scoreboard_selection_state : public main_menu_state {
  public:
	scoreboard_selection_state(std::unique_ptr<playerless_game>&& game, bool returning_from_subscreen);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_SCOREBOARD_SELECTION,
		ENTERING_TIME_SCOREBOARD,
		ENTERING_SCORE_SCOREBOARD,
		EXITING_TO_TITLE
	};

	substate m_substate;

	void set_up_subscreen_animation();
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////////// SCOREBOARDS STATE ////////////////////////////////////////////////////////////

enum class scoreboard {
	TIME = 0,
	SCORE = 4
};

class scoreboard_state : public main_menu_state {
  public:
	scoreboard_state(std::unique_ptr<playerless_game>&& game, scoreboard scoreboard);

	std::unique_ptr<tr::state> update(tr::duration) override;

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

	friend substate operator|(const substate_base& l, const scoreboard& r);
	friend substate_base to_base(substate state);
	friend scoreboard to_scoreboard(substate state);

	void set_up_page_switch_animation();
	void set_up_exit_animation();
};

////////////////////////////////////////////////////////////// REPLAYS STATE //////////////////////////////////////////////////////////////

class replays_state : public main_menu_state {
  public:
	replays_state();
	replays_state(std::unique_ptr<playerless_game>&& game);

  public:
	std::unique_ptr<tr::state> update(tr::duration) override;

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
	std::map<std::string, replay_header>::iterator m_selected;

	float fade_overlay_opacity() override;
	void set_up_ui();
	void set_up_page_switch_animation();
	void set_up_exit_animation();
};

////////////////////////////////////////////////////////////// SETTINGS STATE /////////////////////////////////////////////////////////////

class settings_state : public main_menu_state {
  public:
	settings_state(std::unique_ptr<playerless_game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_SETTINGS,
		ENTERING_TITLE
	};

	substate m_substate;
	settings m_pending;

	void set_up_exit_animation();
};

////////////////////////////////////////////////////////////// CREDITS STATE //////////////////////////////////////////////////////////////

class credits_state : public main_menu_state {
  public:
	credits_state(std::unique_ptr<playerless_game>&& game);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		IN_CREDITS,
		ENTERING_TITLE,
	};

	substate m_substate;
};

/////////////////////////////////////////////////////////////// GAME STATE ////////////////////////////////////////////////////////////////

class game_state : public state {
  public:
	game_state(std::unique_ptr<game>&& game, game_type type, bool fade_in);

	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	std::unique_ptr<tr::state> update(tr::duration) override;
	void draw() override;

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
	std::unique_ptr<game> m_game;

	friend substate operator|(const substate_base& l, const game_type& r);
	friend substate_base to_base(substate state);
	friend game_type to_type(substate state);

	float fade_overlay_opacity() const;
	void add_replay_cursor_to_renderer(glm::vec2 pos) const;
};

/////////////////////////////////////////////////////////////// PAUSE STATE ///////////////////////////////////////////////////////////////

class pause_state : public game_menu_state {
  public:
	pause_state(std::unique_ptr<game>&& game, game_type type, glm::vec2 mouse_pos, bool blur_in);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate_base {
		PAUSING,
		PAUSED,
		UNPAUSING,
		SAVING_AND_RESTARTING,
		RESTARTING,
		SAVING_AND_QUITTING,
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
};

///////////////////////////////////////////////////////////// GAME OVER STATE /////////////////////////////////////////////////////////////

class game_over_state : public game_menu_state {
  public:
	game_over_state(std::unique_ptr<game>&& game, bool blur_in);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate {
		BLURRING_IN,
		GAME_OVER,
		SAVING_AND_RESTARTING,
		RESTARTING,
		SAVING_AND_QUITTING,
		QUITTING
	};

	substate m_substate;

	float fade_overlay_opacity() override;
	float saturation_factor() override;
	float blur_strength() override;

	void set_up_exit_animation();
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
	save_score_state(std::unique_ptr<game>&& game, glm::vec2 mouse_pos, save_screen_flags flags);
	// Used when coming from the game over screen.
	save_score_state(std::unique_ptr<game>&& game, save_screen_flags flags);

	std::unique_ptr<tr::state> update(tr::duration) override;

  private:
	enum class substate_base {
		SAVING_SCORE = 0x0,
		RETURNING = 0x1,
		ENTERING_SAVE_REPLAY = 0x2
	};
	enum class substate {
	}; // substate_base + save_screen_flags

	substate m_substate;
	glm::vec2 m_start_mouse_pos;
	score_entry m_score;

	friend substate operator|(const substate_base& l, const save_screen_flags& r);
	friend substate_base to_base(substate state);
	friend save_screen_flags to_flags(substate state);

	void set_up_ui();
	void set_up_exit_animation();
};

//////////////////////////////////////////////////////////// SAVE REPLAY STATE ////////////////////////////////////////////////////////////

class save_replay_state : public game_menu_state {
  public:
	save_replay_state(std::unique_ptr<game>&& game, save_screen_flags flags);

	std::unique_ptr<tr::state> update(tr::duration) override;

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
};