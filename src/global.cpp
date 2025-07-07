#include "../include/global.hpp"
#include "../include/settings.hpp"
#include <lz4.h>

int max_window_size() noexcept
{
	const glm::ivec2 display_size{tr::display_size()};
	return std::min(display_size.x, display_size.y);
}

std::uint16_t max_refresh_rate() noexcept
{
	return tr::round_cast<std::uint16_t>(tr::refresh_rate());
}

// Gets the formatted timer text string.
std::string timer_text(ticks time)
{
	return {time >= 60_s ? std::format("{}:{:02}:{:02}", time / 60_s, (time % 60_s) / 1_s, (time % 1_s) * 100 / 1_s)
						 : std::format("{:02}:{:02}", time / 1_s, (time % 1_s) * 100 / 1_s)};
}

std::int64_t unix_now() noexcept
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}