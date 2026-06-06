#include "../include/input.hpp"
#include "../include/renderer.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"
#include "../include/state.hpp"
#include "../include/text_engine.hpp"

tr::sys::signal parse_command_line(std::span<tr::cstring_view> args)
{
	return debug_settings::instance().parse(args);
}

tr::sys::signal initialize()
{
	tr::sys::set_app_information("TRDario", "Bodge", VERSION_STRING);
	debug_settings::instance().validate();
	tr::sys::set_draw_frequency(debug_settings::instance().refresh_rate());
	tr::sys::set_tick_frequency(240 * debug_settings::instance().game_speed());

	load_languages();
	load_localization();
	g_text_engine.load_fonts();
	open_window();
	g_renderer.emplace();
	savefile::instance().unnamed() ? g_state.emplace<name_entry_state>() : g_state.emplace<title_state>();
	tr::sys::show_window();
	return tr::sys::signal::CONTINUE;
}

tr::sys::signal handle_event(tr::sys::event& event)
{
	if (event.is<tr::sys::quit_event>()) {
		g_state.clear();
	}
	input::instance().handle_event(event);
	g_state.handle_event(event);
	return !g_state.empty() ? tr::sys::signal::CONTINUE : tr::sys::signal::SUCCESS;
}

tr::sys::signal tick()
{
	g_state.tick();
	return !g_state.empty() ? tr::sys::signal::CONTINUE : tr::sys::signal::SUCCESS;
}

tr::sys::signal draw()
{
	g_renderer->start_benchmark();
	g_state.draw();
	g_renderer->draw_cursor();
	g_renderer->draw_benchmarks();
	g_renderer->stop_benchmark();
	tr::gfx::flip_backbuffer();
	tr::gfx::clear_backbuffer();
	g_renderer->fetch_benchmark();
	return tr::sys::signal::CONTINUE;
}

void shut_down()
{
	g_renderer.reset();
	tr::sys::close_window();
	g_text_engine.unload_fonts();
}