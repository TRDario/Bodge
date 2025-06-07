#include "../include/global.hpp"
#include "../include/settings.hpp"
#include <lz4.h>

int max_window_size() noexcept
{
	const glm::ivec2 display_size{tr::display_size()};
	return min(display_size.x, display_size.y);
}

u16 max_refresh_rate() noexcept
{
	return tr::round_cast<u16>(tr::refresh_rate());
}

u8 max_msaa() noexcept
{
	static u8 max{255};
	if (max == 255) {
		max = 0;
		while (true) {
			try {
				const u8 trying{static_cast<u8>(max == 0 ? 2 : max * 2)};
				tr::window::open_windowed("", {250, 250}, tr::window_flag::DEFAULT, {.multisamples = trying});
				tr::window::close();
				max = trying;
			}
			catch (tr::window_open_error& err) {
				break;
			}
		}
	}
	return max;
}