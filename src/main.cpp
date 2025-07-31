#include "../include/audio.hpp"
#include "../include/fonts.hpp"
#include "../include/global.hpp"
#include "../include/graphics.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"
#include "../include/system.hpp"

int main(int argc, const char** argv)
{
	try {
		tr::initialize_application("TRDario", "Bodge");
		engine::parse_command_line(argc, argv);
		engine::load_settings();
		engine::load_scorefile();
		engine::load_languages();
		engine::load_localization();
		engine::load_fonts();
		engine::initialize_audio();
		engine::initialize_system();
		engine::initialize_graphics();
		engine::set_main_menu_state();
		while (engine::active()) {
			engine::handle_events();
			engine::redraw_if_needed();
		}
		engine::shut_down_graphics();
		engine::shut_down_system();
		engine::shut_down_audio();
		engine::unload_fonts();
		engine::save_scorefile();
		engine::save_settings();
	}
	catch (std::exception& err) {
		LOG(tr::severity::FATAL, err);
		tr::show_fatal_error_message_box(err);
		std::exit(EXIT_FAILURE);
	}
}