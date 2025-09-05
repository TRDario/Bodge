#include "../include/global.hpp"

//

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
	const tm* tm{std::localtime(&timestamp)};
	return TR_FMT::format("{}/{:02}/{:02} {:02}:{:02}", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
}

//

i64 current_timestamp()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//

void fragment::update()
{
	pos += vel / 1_sf;
	rot += rotvel / 1_sf;
}