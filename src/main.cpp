#if defined _MSC_VER and not defined TR_ENABLE_ASSERTS
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#include "../include/audio.hpp"
#include "../include/fonts.hpp"
#include "../include/global.hpp"
#include "../include/graphics.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"
#include "../include/system.hpp"

tr::sys::result tr::sys::user_defined::initialize(std::span<const char*> args)
{
	if (engine::parse_command_line(args) == tr::sys::result::SUCCESS) {
		return result::SUCCESS;
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
	return result::CONTINUE;
}

tr::sys::result tr::sys::user_defined::handle_event(event& event)
{
	engine::handle_event(event);
	return engine::active() ? result::CONTINUE : result::SUCCESS;
}

tr::sys::result tr::sys::user_defined::iterate()
{
	engine::redraw();
	return result::CONTINUE;
}

void tr::sys::user_defined::quit()
{
	engine::shut_down_graphics();
	engine::shut_down_system();
	engine::shut_down_audio();
	engine::unload_fonts();
	engine::save_scorefile();
	engine::save_settings();
}