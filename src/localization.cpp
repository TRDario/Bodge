#include "../include/localization.hpp"
#include "../include/settings.hpp"

void load_language(const std::filesystem::path& entry)
{
	const std::string stem{std::filesystem::path{entry}.stem().string()};
	if (std::filesystem::is_regular_file(entry) && entry.extension() == ".txt" && stem.size() == 2) {
		language_code code{stem[0], stem[1]};
		tr::localization_map temp;
		temp.load(entry);
		std::string name{temp.contains("language_name") ? temp["language_name"] : stem};
		std::string font{temp.contains("font") ? temp["font"] : std::string{}};
		g_languages.insert({code, {std::move(name), std::move(font)}});
	}
}

void load_languages()
{
	try {
		std::ranges::for_each(std::filesystem::directory_iterator{g_cli_settings.data_directory / "localization"}, load_language);
		std::ranges::for_each(std::filesystem::directory_iterator{g_cli_settings.user_directory / "localization"}, load_language);
	}
	catch (std::exception&) {
		g_languages.clear();
	}
}

void load_localization()
{
	if (!g_languages.contains(g_settings.language)) {
		return;
	}

	const std::string_view name{g_settings.language.data(), 2};
	tr::localization_map old{std::move(g_loc)};
	try {
		const std::string filename{TR_FMT::format("localization/{}.txt", name)};
		std::filesystem::path path{g_cli_settings.data_directory / filename};
		if (!std::filesystem::exists(path)) {
			path = g_cli_settings.user_directory / filename;
		}
		g_loc.load(path);
	}
	catch (std::exception&) {
		g_loc = std::move(old);
	}
}