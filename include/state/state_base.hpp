#pragma once
#include "../game/game.hpp"
#include "../ui/ui_manager.hpp"
#include <future>

////////////////////////////////////////////////////////////// STATE MACHINE //////////////////////////////////////////////////////////////

// The global state machine.
inline tr::state_machine g_state_machine;

////////////////////////////////////////////////////////////////// STATE //////////////////////////////////////////////////////////////////

class state : public tr::state {
  public:
	state(selection_tree selection_tree, shortcut_table shortcuts);

	tr::next_state handle_event(const tr::sys::event& event) override;
	tr::next_state tick() override;

  protected:
	ui_manager m_ui;
	ticks m_timer;
	std::future<tr::next_state> m_next_state;

	tr::next_state next_state_if_after(ticks timestamp);
};

///////////////////////////////////////////////////////////// MAIN MENU STATE /////////////////////////////////////////////////////////////

class main_menu_state : public state {
  public:
	main_menu_state(selection_tree selection_tree, shortcut_table shortcuts);
	main_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<playerless_game> game);

	tr::next_state tick() override;
	void draw() override;

  protected:
	std::shared_ptr<playerless_game> m_game;

  private:
	virtual float fade_overlay_opacity();
};

///////////////////////////////////////////////////////////// GAME MENU STATE /////////////////////////////////////////////////////////////

class game_menu_state : public state {
  public:
	game_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<game> game, bool update_game);

	tr::next_state tick() override;
	void draw() override;

  protected:
	std::shared_ptr<game> m_game;

  private:
	bool m_update_game;

	virtual float saturation_factor();
	virtual float blur_strength();
	virtual float fade_overlay_opacity();
};

/////////////////////////////////////////////////////////////// MAKE ASYNC ////////////////////////////////////////////////////////////////

template <class T, class... Ts>
std::future<tr::next_state> make_async(Ts&&... args)
	requires(std::constructible_from<T, Ts...>)
{
	return std::async(
		std::launch::async, []<class... Us>(Us&&... args) { return (tr::next_state)std::make_unique<T>(std::forward<Us>(args)...); },
		std::forward<Ts>(args)...);
}