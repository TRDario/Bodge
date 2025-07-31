#include "../include/audio.hpp"
#include "../include/engine.hpp"
#include "../include/fonts.hpp"
#include "../include/global.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"

int main(int argc, const char** argv)
{
	try {
		tr::initialize_application("TRDario", "Bodge");
		cli_settings.parse(argc, argv);
		settings.load_from_file();
		scorefile.load_from_file();
		load_languages();
		load_localization();
		fonts::load();
		audio::initialize();
		engine::initialize();
		engine::set_main_menu_state();
		while (engine::active()) {
			engine::handle_events();
			engine::redraw_if_needed();
		}
		engine::shut_down();
		audio::shut_down();
		fonts::unload_all();
		scorefile.save_to_file();
		settings.save_to_file();
	}
	catch (std::exception& err) {
		LOG(tr::severity::FATAL, err);
		tr::show_fatal_error_message_box(err);
		std::exit(EXIT_FAILURE);
	}
}