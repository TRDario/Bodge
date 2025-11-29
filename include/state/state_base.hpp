#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

//////////////////////////////////////////////////////////////// NEXT STATE ////////////////////////////////////////////////////////////////

class name_entry_state;
class title_state;
class start_game_state;
class gamemode_designer_state;
class ball_settings_editor_state;
class player_settings_editor_state;
class scoreboard_selection_state;
class scoreboard_state;
class replays_state;
class settings_state;
class credits_state;
class game_state;
class pause_state;
class game_over_state;
class save_score_state;
class save_replay_state;
using next_state = tr::next_state<name_entry_state, title_state, start_game_state, gamemode_designer_state, ball_settings_editor_state,
								  player_settings_editor_state, scoreboard_selection_state, scoreboard_state, replays_state, settings_state,
								  credits_state, game_state, pause_state, game_over_state, save_score_state, save_replay_state>;

class state {
  public:
	state(selection_tree selection_tree, shortcut_table shortcuts);

	next_state handle_event(const tr::sys::event& event);
	next_state tick();

  protected:
	ui_manager m_ui;
	ticks m_timer;
};

///////////////////////////////////////////////////////////// MAIN MENU STATE /////////////////////////////////////////////////////////////

class main_menu_state : public state {
  public:
	main_menu_state(selection_tree selection_tree, shortcut_table shortcuts);
	main_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<playerless_game> game);
	main_menu_state(main_menu_state&&) noexcept = default;
	main_menu_state& operator=(main_menu_state&&) noexcept = default;
	virtual ~main_menu_state() = default;

	next_state tick();
	void draw();

  protected:
	std::shared_ptr<playerless_game> m_game;

  private:
	virtual float fade_overlay_opacity();
};

///////////////////////////////////////////////////////////// GAME MENU STATE /////////////////////////////////////////////////////////////

class game_menu_state : public state {
  public:
	game_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<game> game, bool update_game);
	game_menu_state(game_menu_state&&) noexcept = default;
	game_menu_state& operator=(game_menu_state&&) noexcept = default;
	virtual ~game_menu_state() = default;

	next_state tick();
	void draw();

  protected:
	std::shared_ptr<game> m_game;

  private:
	bool m_update_game;

	virtual float saturation_factor();
	virtual float blur_strength();
	virtual float fade_overlay_opacity();
};