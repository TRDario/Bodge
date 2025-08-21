#include "../include/global.hpp"

// Gets the formatted timer text string.
std::string timer_text(ticks time)
{
	return {time >= 60_s ? std::format("{}:{:02}:{:02}", time / 60_s, (time % 60_s) / 1_s, (time % 1_s) * 100 / 1_s)
						 : std::format("{:02}:{:02}", time / 1_s, (time % 1_s) * 100 / 1_s)};
}

std::int64_t unix_now()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}