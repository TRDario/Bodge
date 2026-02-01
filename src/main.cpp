#include "../include/audio.hpp"
#include "../include/renderer.hpp"
#include "../include/score.hpp"
#include "../include/settings.hpp"
#include "../include/state.hpp"
#include "../include/text_engine.hpp"

tr::sys::signal parse_command_line(std::span<tr::cstring_view> args)
{
	for (auto it = args.begin(); it != args.end(); ++it) {
		if (*it == "--datadir" && ++it < args.end()) {
			g_cli_settings.data_directory = std::filesystem::canonical(std::filesystem::path{*it});
		}
		else if (*it == "--userdir" && ++it < args.end()) {
			const std::filesystem::path userdir_path{std::filesystem::path{*it}};
			if (!std::filesystem::exists(userdir_path)) {
				std::filesystem::create_directory(userdir_path);
			}
			g_cli_settings.user_directory = std::filesystem::canonical(userdir_path);
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
			std::cout << "Bodge " VERSION_STRING " by TRDario, 2025.\n"
						 "Supported arguments:\n"
						 "--datadir <path>       - Overrides the data directory.\n"
						 "--userdir <path>       - Overrides the user directory.\n"
						 "--refreshrate <number> - Overrides the refresh rate.\n"
						 "--gamespeed <factor>   - Overrides the speed multiplier.\n"
						 "--showperf             - Shows performance information.\n";
			return tr::sys::signal::SUCCESS;
		}
	}
	return tr::sys::signal::CONTINUE;
}

tr::sys::signal initialize()
{
	tr::sys::set_app_information("TRDario", "Bodge", VERSION_STRING);
	if (g_cli_settings.data_directory.empty()) {
		g_cli_settings.data_directory = tr::sys::executable_dir() / "data";
	}
	if (g_cli_settings.user_directory.empty()) {
		g_cli_settings.user_directory = tr::sys::user_dir();
	}
	constexpr std::array<tr::cstring_view, 5> DIRECTORIES{"localization", "fonts", "music", "replays", "gamemodes"};
	for (tr::cstring_view directory : DIRECTORIES) {
		const std::filesystem::path path{g_cli_settings.user_directory / directory};
		if (!std::filesystem::is_directory(path)) {
			std::filesystem::create_directory(path);
		}
	}

	g_cli_settings.refresh_rate = std::clamp(g_cli_settings.refresh_rate, 1.0f, tr::sys::refresh_rate());
	tr::sys::set_draw_frequency(g_cli_settings.refresh_rate);
	tr::sys::set_tick_frequency(240);

	g_settings.load_from_file();
	g_scorefile.load_from_file();
	load_languages();
	load_localization();
	g_text_engine.load_fonts();
	g_audio.initialize();
	open_window();
	g_renderer.emplace();
	g_scorefile.name.empty() ? g_state_machine.emplace<name_entry_state>() : g_state_machine.emplace<title_state>();
	tr::sys::show_window();
	return tr::sys::signal::CONTINUE;
}

tr::sys::signal handle_event(tr::sys::event& event)
{
	if (event.is<tr::sys::quit_event>()) {
		g_state_machine.clear();
	}
	else if (event.is<tr::sys::window_gain_focus_event>()) {
		tr::sys::set_mouse_relative_mode(true);
	}
	else if (event.is<tr::sys::window_lose_focus_event>()) {
		tr::sys::set_mouse_relative_mode(false);
	}
	else if (event.is<tr::sys::key_down_event>()) {
		g_held_keymods = event.as<tr::sys::key_down_event>().mods;
	}
	else if (event.is<tr::sys::key_up_event>()) {
		g_held_keymods = event.as<tr::sys::key_up_event>().mods;
	}
	else if (event.is<tr::sys::mouse_motion_event>() && tr::sys::window_has_focus()) {
		const float multiplier{g_settings.mouse_sensitivity / 100.0f / g_renderer->scale() * tr::sys::window_pixel_density()};
		const glm::vec2 delta{event.as<tr::sys::mouse_motion_event>().delta * multiplier};
		g_mouse_pos = glm::clamp(g_mouse_pos + delta, 0.0f, 1000.0f);
	}
	else if (event.is<tr::sys::mouse_down_event>()) {
		g_held_buttons |= event.as<tr::sys::mouse_down_event>().button;
	}
	else if (event.is<tr::sys::mouse_up_event>()) {
		g_held_buttons &= ~event.as<tr::sys::mouse_up_event>().button;
	}

	g_state_machine.handle_event(event);
	return !g_state_machine.empty() ? tr::sys::signal::CONTINUE : tr::sys::signal::SUCCESS;
}

tr::sys::signal tick()
{
	g_state_machine.tick();
	return !g_state_machine.empty() ? tr::sys::signal::CONTINUE : tr::sys::signal::SUCCESS;
}

tr::sys::signal draw()
{
	if (g_cli_settings.show_perf) {
		g_renderer->extra->benchmark.start();
	}
	g_state_machine.draw();
	g_renderer->draw_cursor();
	if (g_cli_settings.show_perf) {
		g_renderer->extra->debug.write_right(g_state_machine.tick_benchmark(), "Tick:", 1.0s / 1_s);
		g_renderer->extra->debug.newline_right();
		g_renderer->extra->debug.write_right(g_state_machine.draw_benchmark(), "Render (CPU):", 1.0s / g_cli_settings.refresh_rate);
		g_renderer->extra->debug.newline_right();
		g_renderer->extra->debug.write_right(g_renderer->extra->benchmark, "Render (GPU):", 1.0s / g_cli_settings.refresh_rate);
		g_renderer->extra->debug.draw();
	}
	if (g_cli_settings.show_perf) {
		g_renderer->extra->benchmark.stop();
	}
	tr::gfx::flip_backbuffer();
	tr::gfx::clear_backbuffer();
	if (g_cli_settings.show_perf) {
		g_renderer->extra->benchmark.fetch();
	}
	return tr::sys::signal::CONTINUE;
}

void shut_down()
{
	g_renderer.reset();
	tr::sys::close_window();
	g_audio.shut_down();
	g_text_engine.unload_fonts();
	g_scorefile.save_to_file();
	g_settings.save_to_file();
}