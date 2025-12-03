#include "../../include/state/state_base.hpp"
#include "../../include/graphics.hpp"

////////////////////////////////////////////////////////////////// STATE //////////////////////////////////////////////////////////////////

state::state(selection_tree selection_tree, shortcut_table shortcuts)
	: m_ui{selection_tree, shortcuts}, m_timer{0}
{
}

tr::next_state state::handle_event(const tr::sys::event& event)
{
	m_ui.handle_event(event);
	return tr::KEEP_STATE;
}

tr::next_state state::tick()
{
	m_ui.update();
	++m_timer;
	return tr::KEEP_STATE;
}

tr::next_state state::next_state_if_after(ticks timestamp)
{
	return m_timer >= timestamp ? std::optional{m_next_state.get()} : tr::KEEP_STATE;
}

///////////////////////////////////////////////////////////// MAIN MENU STATE /////////////////////////////////////////////////////////////

main_menu_state::main_menu_state(selection_tree selection_tree, shortcut_table shortcuts)
	: state{selection_tree, shortcuts}, m_game{std::make_shared<playerless_game>(pick_menu_gamemode(), g_rng.generate<u64>())}
{
}

main_menu_state::main_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<playerless_game> game)
	: state{selection_tree, shortcuts}, m_game{std::move(game)}
{
}

float main_menu_state::fade_overlay_opacity()
{
	return 0;
}

tr::next_state main_menu_state::tick()
{
	state::tick();
	m_game->update();
	return tr::KEEP_STATE;
}

void main_menu_state::draw()
{
	m_game->add_to_renderer();
	g_graphics->add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	g_graphics->add_fade_overlay_to_renderer(fade_overlay_opacity());
	g_graphics->basic_renderer.draw(g_graphics->screen);
}

///////////////////////////////////////////////////////////// GAME MENU STATE /////////////////////////////////////////////////////////////

game_menu_state::game_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<game> game, bool update_game)
	: state{selection_tree, shortcuts}, m_game{std::move(game)}, m_update_game{update_game}
{
}

tr::next_state game_menu_state::tick()
{
	state::tick();
	if (m_update_game) {
		m_game->update();
	}
	return tr::KEEP_STATE;
}

void game_menu_state::draw()
{
	if (m_update_game) {
		m_game->add_to_renderer();
		g_graphics->basic_renderer.draw(g_graphics->blur_renderer.input());
	}
	g_graphics->blur_renderer.draw(saturation_factor(), blur_strength());
	m_ui.add_to_renderer();
	g_graphics->add_fade_overlay_to_renderer(fade_overlay_opacity());
	g_graphics->basic_renderer.draw(g_graphics->screen);
}

float game_menu_state::saturation_factor()
{
	return 0.35f;
}

float game_menu_state::blur_strength()
{
	return 10.0f;
}

float game_menu_state::fade_overlay_opacity()
{
	return 0;
}