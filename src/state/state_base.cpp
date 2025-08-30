#include "../../include/state/state_base.hpp"
#include "../../include/graphics.hpp"

////////////////////////////////////////////////////////////////// STATE //////////////////////////////////////////////////////////////////

state::state(selection_tree selection_tree, shortcut_table shortcuts)
	: m_ui{selection_tree, shortcuts}, m_timer{0}
{
}

std::unique_ptr<tr::state> state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> state::update(tr::duration)
{
	m_ui.update();
	++m_timer;
	return nullptr;
}

///////////////////////////////////////////////////////////// MAIN MENU STATE /////////////////////////////////////////////////////////////

main_menu_state::main_menu_state(selection_tree selection_tree, shortcut_table shortcuts)
	: state{selection_tree, shortcuts}
	, m_background_game{std::make_unique<playerless_game>(engine::pick_menu_gamemode(), engine::rng.generate<u64>())}
{
}

main_menu_state::main_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::unique_ptr<playerless_game>&& game)
	: state{selection_tree, shortcuts}, m_background_game{std::move(game)}
{
}

std::unique_ptr<playerless_game>&& main_menu_state::release_game()
{
	return std::move(m_background_game);
}

float main_menu_state::fade_overlay_opacity()
{
	return 0;
}

std::unique_ptr<tr::state> main_menu_state::update(tr::duration)
{
	state::update({});
	m_background_game->update();
	return nullptr;
}

void main_menu_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////// GAME MENU STATE /////////////////////////////////////////////////////////////

game_menu_state::game_menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::unique_ptr<game>&& game, bool update_game)
	: state{selection_tree, shortcuts}, m_game{std::move(game)}, m_update_game{update_game}
{
}

std::unique_ptr<game>&& game_menu_state::release_game()
{
	return std::move(m_game);
}

std::unique_ptr<tr::state> game_menu_state::update(tr::duration)
{
	state::update({});
	if (m_update_game) {
		m_game->update();
	}
	return nullptr;
}

void game_menu_state::draw()
{
	if (m_update_game) {
		m_game->add_to_renderer();
		tr::gfx::renderer_2d::draw(engine::blur().input());
	}
	engine::blur().draw(saturation_factor(), blur_strength());
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::gfx::renderer_2d::draw(engine::screen());
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