#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"

////////////////////////////////////////////////////////////////// STATE //////////////////////////////////////////////////////////////////

class state : public tr::state {
  public:
	state(selection_tree selection_tree, shortcut_table shortcuts);

	std::unique_ptr<tr::state> handle_event(const tr::system::event& event) override;
	std::unique_ptr<tr::state> update(tr::duration) override;

  protected:
	ui_manager m_ui;
	ticks m_timer;
};

///////////////////////////////////////////////////////////// MAIN MENU STATE /////////////////////////////////////////////////////////////

class main_menu_state : public state {
  public:
	main_menu_state(selection_tree selection_tree, shortcut_table shortcuts);
	main_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::unique_ptr<playerless_game>&& game);

	std::unique_ptr<playerless_game>&& release_game();

	std::unique_ptr<tr::state> update(tr::duration) override;
	void draw() override;

  private:
	std::unique_ptr<playerless_game> m_background_game;

	virtual float fade_overlay_opacity();
};

///////////////////////////////////////////////////////////// GAME MENU STATE /////////////////////////////////////////////////////////////

class game_menu_state : public state {
  public:
	game_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::unique_ptr<game>&& game, bool update_game);

	std::unique_ptr<game>&& release_game();

	std::unique_ptr<tr::state> update(tr::duration) override;
	void draw() override;

  protected:
	std::unique_ptr<game> m_game;

  private:
	bool m_update_game;

	virtual float saturation_factor();
	virtual float blur_strength();
	virtual float fade_overlay_opacity();
};