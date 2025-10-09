#if defined _MSC_VER and not defined TR_ENABLE_ASSERTS
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#include "../include/audio.hpp"
#include "../include/fonts.hpp"
#include "../include/graphics.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"
#include "../include/system.hpp"

tr::sys::signal initialize(std::span<const char*> args)
{
	if (engine::parse_command_line(args) == tr::sys::signal::SUCCESS) {
		return tr::sys::signal::SUCCESS;
	}
	engine::load_settings();
	engine::load_scorefile();
	engine::load_languages();
	engine::load_localization();
	engine::load_fonts();
	engine::initialize_audio();
	engine::initialize_system();
	engine::initialize_graphics();
	engine::set_main_menu_state();
	return tr::sys::signal::CONTINUE;
}

tr::sys::signal handle_event(tr::sys::event& event)
{
	engine::handle_event(event);
	return engine::active() ? tr::sys::signal::CONTINUE : tr::sys::signal::SUCCESS;
}

tr::sys::signal iterate()
{
	engine::redraw();
	return tr::sys::signal::CONTINUE;
}

void quit()
{
	engine::shut_down_graphics();
	engine::shut_down_system();
	engine::shut_down_audio();
	engine::unload_fonts();
	engine::save_scorefile();
	engine::save_settings();
}