#include "../include/system.hpp"
#include "../include/audio.hpp"
#include "../include/graphics.hpp"
#include "../include/settings.hpp"
#include "../include/state/state.hpp"
#include <SDL3/SDL.h>

bool engine::restart_required(const ::settings& old)
{
	return old.display_mode != g_settings.display_mode ||
		   (g_settings.display_mode == display_mode::WINDOWED && old.window_size != g_settings.window_size) || old.msaa != g_settings.msaa;
}

///////////////////////////////////////////////////////////////// LIFETIME ////////////////////////////////////////////////////////////////

void engine::initialize_system()
{
	const tr::gfx::properties gfx{
		.multisamples = g_settings.msaa,
	};
	if (g_settings.display_mode == display_mode::FULLSCREEN) {
		tr::sys::open_fullscreen_window("Bodge", tr::sys::NOT_RESIZABLE, gfx);
	}
	else {
		tr::sys::open_window("Bodge", glm::ivec2{g_settings.window_size}, tr::sys::NOT_RESIZABLE, gfx);
	}
	tr::sys::set_window_icon(tr::load_bitmap_file(g_cli_settings.data_directory / "graphics" / "icon.qoi"));
	tr::sys::set_window_vsync(g_settings.vsync ? tr::sys::vsync::ADAPTIVE : tr::sys::vsync::DISABLED);
	tr::sys::set_draw_frequency(g_settings.vsync ? 0 : g_cli_settings.refresh_rate);
	tr::sys::raise_window();
}

void engine::apply_settings(const ::settings& old)
{
	if (restart_required(old)) {
		auto temp{std::move(g_state_machine)};
		g_graphics.reset();
		tr::sys::close_window();
		initialize_system();
		g_graphics.emplace();
		g_state_machine = std::move(temp);
	}
	g_audio.apply_settings();
}