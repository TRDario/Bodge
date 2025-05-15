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

std::uint8_t max_msaa() noexcept
{
	static std::uint8_t max{255};
	if (max == 255) {
		max = 0;
		while (true) {
			try {
				const std::uint8_t trying{static_cast<std::uint8_t>(max == 0 ? 2 : max * 2)};
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

//

void load_localization() noexcept
{
	const std::string_view name{settings.language};
	tr::localization_map old{std::move(localization)};
	try {
		std::string filename{std::format("localization/{}.loc", name)};
		std::filesystem::path path{cli_settings.datadir / filename};
		if (!exists(path)) {
			path = cli_settings.userdir / filename;
			if (!exists(path)) {
				throw tr::file_not_found{std::move(filename)};
			}
		}

		std::vector<std::string> errors{localization.load(path)};
		if (errors.empty()) {
			LOG(tr::severity::INFO, "Loaded '{}' localization from '{}'.", name, path.string());
		}
		else {
			LOG(tr::severity::WARN, "Loaded '{}' localization from '{}' with {} errors: ", name, path.string(), errors.size());
			for (auto& error : errors) {
				LOG(tr::severity::ERROR, "{}", error);
			}
		}
	}
	catch (std::exception& err) {
		localization = std::move(old);
		LOG(tr::severity::ERROR, "Failed to load '{}' localization: {}", name, err.what());
	}
}

std::string get_language_font(language_code code)
{
	std::string filename{std::format("localization/{}.loc", std::string_view{code})};
	std::filesystem::path path{cli_settings.datadir / filename};
	if (!exists(path)) {
		path = cli_settings.userdir / filename;
		if (!exists(path)) {
			LOG(tr::severity::ERROR, "Couldn't determine language font because '{}' doesn't exist.", filename);
			throw tr::file_not_found{std::move(filename)};
		}
	}

	tr::localization_map temp;
	temp.load(path);
	if (temp.contains("font")) {
		return std::string{temp["font"]};
	}
	else {
		return {};
	}
}