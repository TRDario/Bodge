///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements state/state_base.hpp.                                                                                                      //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state/state_base.hpp"
#include "../../include/renderer.hpp"
#include "../../include/state.hpp"

////////////////////////////////////////////////////////////////// STATE //////////////////////////////////////////////////////////////////

state::subsystems::subsystems()
	: localization{settings.language}, audio{settings}, renderer{localization, settings}
{
}

//

state::state(std::shared_ptr<subsystems> subsystems, selection_tree selection_tree, shortcut_table shortcuts)
	: m_subsystems{std::move(subsystems)}, m_ui{m_subsystems->audio, selection_tree, shortcuts}, m_elapsed{0}
{
}

bool state::transparent_cursor() const
{
	return false;
}

tr::next_state state::handle_event(const tr::sys::event& event)
{
	if (event.is<tr::sys::quit_event>()) {
		return tr::DROP_STATE;
	}

	const float scale{m_subsystems->renderer.scale() * tr::sys::window_pixel_density()};
	const float mouse_sensitivity{m_subsystems->settings.mouse_sensitivity / 100.0f / scale};
	m_subsystems->input.handle_event(event, mouse_sensitivity);
	m_ui.handle_event(m_subsystems->input, event);
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

main_menu_state::main_menu_state(std::shared_ptr<subsystems> subsystems, selection_tree selection_tree, shortcut_table shortcuts)
	: state{std::move(subsystems), selection_tree, shortcuts}
	, m_game{std::make_shared<playerless_game>(pick_menu_gamemode(), g_rng.generate<u64>())}
{
}

main_menu_state::main_menu_state(std::shared_ptr<subsystems> subsystems, selection_tree selection_tree, shortcut_table shortcuts,
								 std::shared_ptr<playerless_game> game)
	: state{std::move(subsystems), selection_tree, shortcuts}, m_game{std::move(game)}
{
}

float main_menu_state::fade_overlay_opacity()
{
	return 0;
}

tr::next_state main_menu_state::tick()
{
	state::tick();
	m_game->tick(m_subsystems->audio);
	return tr::KEEP_STATE;
}

void main_menu_state::draw()
{
	renderer& renderer{m_subsystems->renderer};
	renderer.start_benchmark();
	m_game->add_to_renderer(renderer, m_subsystems->settings.secondary_hue);
	renderer.add_menu_game_overlay();
	m_ui.add_to_renderer(renderer, m_subsystems->input.mouse_pos);
	renderer.add_fade_overlay(fade_overlay_opacity());
	renderer.draw_cursor(m_subsystems->settings.primary_hue, m_subsystems->input.mouse_pos);
	renderer.draw_layers(renderer.screen());
	renderer.draw_benchmarks(debug_settings::instance().refresh_rate(), current_state::instance().tick_benchmark(),
							 current_state::instance().draw_benchmark());
	renderer.stop_benchmark();
	tr::gfx::flip_backbuffer();
	tr::gfx::clear_backbuffer();
	renderer.fetch_benchmark();
}

///////////////////////////////////////////////////////////// GAME MENU STATE /////////////////////////////////////////////////////////////

game_menu_state::game_menu_state(std::shared_ptr<subsystems> subsystems, selection_tree selection_tree, shortcut_table shortcuts,
								 std::shared_ptr<game> game, savefile savefile, update_game update_game)
	: state{std::move(subsystems), selection_tree, shortcuts}
	, m_game{std::move(game)}
	, m_savefile{std::move(savefile)}
	, m_update_game{bool(update_game)}
{
}

tr::next_state game_menu_state::tick()
{
	state::tick();
	if (m_update_game) {
		m_game->tick(m_subsystems->audio, m_subsystems->renderer);
	}
	return tr::KEEP_STATE;
}

void game_menu_state::draw()
{
	renderer& renderer{m_subsystems->renderer};
	renderer.start_benchmark();
	if (m_update_game) {
		m_game->add_to_renderer(renderer, m_subsystems->settings.primary_hue, m_subsystems->settings.secondary_hue);
		renderer.draw_layers(renderer.blur_input());
	}
	renderer.draw_blurred(saturation_factor(), blur_strength());
	m_ui.add_to_renderer(renderer, m_subsystems->input.mouse_pos);
	renderer.add_fade_overlay(fade_overlay_opacity());
	renderer.draw_cursor(m_subsystems->settings.primary_hue, m_subsystems->input.mouse_pos);
	renderer.draw_layers(renderer.screen());
	renderer.draw_benchmarks(debug_settings::instance().refresh_rate(), current_state::instance().tick_benchmark(),
							 current_state::instance().draw_benchmark());
	renderer.stop_benchmark();
	tr::gfx::flip_backbuffer();
	tr::gfx::clear_backbuffer();
	renderer.fetch_benchmark();
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