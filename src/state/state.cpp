#include "../../include/state/state.hpp"
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

/////////////////////////////////////////////////////////////// MENU STATE ////////////////////////////////////////////////////////////////

menu_state::menu_state(selection_tree selection_tree, shortcut_table shortcuts)
	: state{selection_tree, shortcuts}
	, m_background_game{std::make_unique<game>(pick_menu_gamemode(), engine::rng.generate<std::uint64_t>())}
{
}

menu_state::menu_state(selection_tree selection_tree, shortcut_table shortcuts, std::unique_ptr<game>&& game)
	: state{selection_tree, shortcuts}, m_background_game{std::move(game)}
{
}

std::unique_ptr<game>&& menu_state::release_game()
{
	return std::move(m_background_game);
}

float menu_state::fade_overlay_opacity()
{
	return 0;
}

std::unique_ptr<tr::state> menu_state::update(tr::duration)
{
	state::update({});
	m_background_game->update();
	return nullptr;
}

void menu_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::gfx::renderer_2d::draw(engine::screen());
}