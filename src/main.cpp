#if defined _MSC_VER and not defined TR_ENABLE_ASSERTS
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

#include "../include/audio.hpp"
#include "../include/fonts.hpp"
#include "../include/graphics.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"
#include "../include/system.hpp"

tr::sys::signal parse_command_line(std::span<tr::cstring_view> args)
{
	for (auto it = args.begin(); it != args.end(); ++it) {
		if (*it == "--datadir" && ++it < args.end()) {
			g_cli_settings.data_directory = std::filesystem::canonical(std::filesystem::path{*it});
		}
		else if (*it == "--userdir" && ++it < args.end()) {
			g_cli_settings.user_directory = std::filesystem::canonical(std::filesystem::path{*it});
		}
		else if (*it == "--refreshrate" && ++it < args.end()) {
			std::from_chars(*it, *it + std::strlen(*it), g_cli_settings.refresh_rate);
		}
		else if (*it == "--gamespeed" && ++it < args.end()) {
			std::from_chars(*it, *it + std::strlen(*it), g_cli_settings.game_speed);
		}
		else if (*it == "--showperf") {
			g_cli_settings.show_perf = true;
		}
		else if (*it == "--help") {
			std::cout << "Bodge v1.2.0 by TRDario, 2025.\n"
						 "Supported arguments:\n"
						 "--datadir <path>       - Overrides the data directory.\n"
						 "--userdir <path>       - Overrides the user directory.\n"
						 "--refreshrate <number> - Overrides the refresh rate.\n"
						 "--gamespeed <factor>   - Overrides the speed multiplier.\n"
						 "--showperf             - Shows performance information.\n";
			return tr::sys::signal::SUCCESS;
		}
	}

	tr::sys::set_app_information("TRDario", "Bodge", VERSION_STRING);
	if (g_cli_settings.data_directory.empty()) {
		g_cli_settings.data_directory = tr::sys::executable_dir() / "data";
	}
	if (g_cli_settings.user_directory.empty()) {
		g_cli_settings.user_directory = tr::sys::user_dir();
	}
	g_cli_settings.refresh_rate = std::clamp(g_cli_settings.refresh_rate, 1.0f, tr::sys::refresh_rate());

	constexpr std::array<tr::cstring_view, 5> DIRECTORIES{"localization", "fonts", "music", "replays", "gamemodes"};
	for (tr::cstring_view directory : DIRECTORIES) {
		const std::filesystem::path path{g_cli_settings.user_directory / directory};
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
		}
	}

	return tr::sys::signal::CONTINUE;
}

tr::sys::signal initialize()
{
	g_settings.load_from_file();
	g_scorefile.load_from_file();
	engine::load_languages();
	engine::load_localization();
	engine::load_fonts();
	g_audio.initialize();
	engine::initialize_system();
	engine::initialize_graphics();
	engine::set_main_menu_state();
	tr::sys::set_tick_frequency(240);
	return tr::sys::signal::CONTINUE;
}

tr::sys::signal handle_event(tr::sys::event& event)
{
	engine::handle_event(event);
	return engine::active() ? tr::sys::signal::CONTINUE : tr::sys::signal::SUCCESS;
}

tr::sys::signal draw()
{
	engine::redraw();
	return tr::sys::signal::CONTINUE;
}

void shut_down()
{
	engine::shut_down_graphics();
	engine::shut_down_system();
	g_audio.shut_down();
	engine::unload_fonts();
	g_scorefile.save_to_file();
	g_settings.save_to_file();
}