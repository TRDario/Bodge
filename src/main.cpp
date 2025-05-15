#include "../include/audio.hpp"
#include "../include/engine.hpp"
#include "../include/font_manager.hpp"
#include "../include/global.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"

int main(int argc, const char** argv)
{
	try {
		cli_settings.parse(argc, argv);
		settings.load_from_file();
		scorefile.load_from_file();
		load_localization();
		font_manager.load_fonts();
		audio::initialize();
		engine::initialize();
		while (engine::active()) {
			engine::handle_events();
			engine::redraw_if_needed();
		}
		engine::shutdown();
		audio::shut_down();
		font_manager.unload_all();
		scorefile.save_to_file();
		settings.save_to_file();
	}
	catch (std::exception& err) {
		LOG(tr::severity::FATAL, "Fatal exception: {}", err.what());
		tr::show_message_box(tr::msg_box_type::ERROR, tr::msg_buttons::OK, "Fatal Exception - Bodge", err.what());
		std::exit(EXIT_FAILURE);
	}
}