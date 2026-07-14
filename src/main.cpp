#include "../include/settings.hpp"
#include "../include/state.hpp"

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
	return tr::sys::signal::CONTINUE;
}

tr::sys::signal handle_event(tr::sys::event& event)
{
	return current_state::instance().handle_event(event);
}

tr::sys::signal tick()
{
	return current_state::instance().tick();
}

tr::sys::signal draw()
{
	current_state::instance().draw();
	return tr::sys::signal::CONTINUE;
}

void shut_down() {}