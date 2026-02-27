///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements global.hpp.                                                                                                                //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/settings.hpp"

///////////////////////////////////////////////////////////////// TICKRATE ////////////////////////////////////////////////////////////////

std::string format_score(i64 score)
{
	return TR_FMT::format("{:05}", score);
}

std::string format_time(ticks time)
{
	return {time >= 60_s ? TR_FMT::format("{}:{:02}:{:02}", time / 60_s, (time % 60_s) / 1_s, (time % 1_s) * 100 / 1_s)
						 : TR_FMT::format("{:02}:{:02}", time / 1_s, (time % 1_s) * 100 / 1_s)};
}

std::string format_time_long(ticks time)
{
	return TR_FMT::format("{}:{:02}:{:02}", time / 60_s, (time % 60_s) / 1_s, (time % 1_s) * 100 / 1_s);
}

std::string format_playtime(ticks playtime)
{
	return TR_FMT::format("{}:{:02}:{:02}", playtime / 3600_s, playtime % 3600_s / 60_s, playtime % 60_s / 1_s);
}

std::string format_timestamp(i64 timestamp)
{
	const std::tm time{tr::localtime(timestamp)};
	return TR_FMT::format("{}/{:02}/{:02} {:02}:{:02}", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min);
}

////////////////////////////////////////////////////////////// MISCELLANEOUS //////////////////////////////////////////////////////////////

i64 current_timestamp()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//

void fragment::tick()
{
	pos += vel / 1_sf;
	rot += rotvel / 1_sf;
}

//

void open_window()
{
	const tr::gfx::properties gfx{.multisamples = tr::sys::max_msaa()};
	if (g_settings.display_mode == display_mode::FULLSCREEN) {
		tr::sys::open_fullscreen_window("Bodge", tr::sys::NOT_RESIZABLE, gfx);
	}
	else {
		tr::sys::open_window("Bodge", glm::ivec2{g_settings.window_size}, tr::sys::NOT_RESIZABLE, gfx);
	}
	tr::sys::set_window_icon(tr::load_bitmap_file(g_cli_settings.data_directory / "graphics" / "icon.qoi"));
	tr::sys::set_window_vsync(g_settings.vsync ? tr::sys::vsync::ADAPTIVE : tr::sys::vsync::DISABLED);
	tr::sys::raise_window();
}