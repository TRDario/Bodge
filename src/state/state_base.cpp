///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements state/state_base.hpp.                                                                                                      //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state/state_base.hpp"
#include "../../include/renderer.hpp"

////////////////////////////////////////////////////////////////// STATE //////////////////////////////////////////////////////////////////

state::state(selection_tree selection_tree, shortcut_table shortcuts)
	: m_ui{selection_tree, shortcuts}, m_elapsed{0}
{
}

tr::next_state state::handle_event(const tr::sys::event& event)
{
	m_ui.handle_event(event);
	return tr::KEEP_STATE;
}

tr::next_state state::tick()
{
	m_ui.tick();
	++m_elapsed;
	return tr::KEEP_STATE;
}

tr::next_state state::next_state_if_after(ticks timestamp)
{
	return m_elapsed >= timestamp && m_next_state.wait_for(0s) == std::future_status::ready ? std::optional{m_next_state.get()}
																							: tr::KEEP_STATE;
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
	m_game->tick();
	return tr::KEEP_STATE;
}

void main_menu_state::draw()
{
	m_game->add_to_renderer();
	g_renderer->add_menu_game_overlay();
	m_ui.add_to_renderer();
	g_renderer->add_fade_overlay(fade_overlay_opacity());
	g_renderer->draw_layers(g_renderer->screen);
}

///////////////////////////////////////////////////////////// GAME MENU STATE /////////////////////////////////////////////////////////////

game_menu_state::game_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::shared_ptr<game> game,
								 update_game update_game)
	: state{selection_tree, shortcuts}, m_game{std::move(game)}, m_update_game{bool(update_game)}
{
}

tr::next_state game_menu_state::tick()
{
	state::tick();
	if (m_update_game) {
		m_game->tick();
	}
	return tr::KEEP_STATE;
}

void game_menu_state::draw()
{
	if (m_update_game) {
		m_game->add_to_renderer();
		g_renderer->draw_layers(g_renderer->blur.input());
	}
	g_renderer->blur.draw(saturation_factor(), blur_strength());
	m_ui.add_to_renderer();
	g_renderer->add_fade_overlay(fade_overlay_opacity());
	g_renderer->draw_layers(g_renderer->screen);
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