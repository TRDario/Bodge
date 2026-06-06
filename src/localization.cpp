///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements localization.hpp.                                                                                                          //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/localization.hpp"
#include "../include/settings.hpp"

///////////////////////////////////////////////////////////// INTERNAL HELPERS ////////////////////////////////////////////////////////////

// Loads language information from file.
static void load_language(std::map<language_code, language_info>& available_languages, const std::filesystem::path& entry)
{
	const std::string stem{entry.stem().string()};
	if (std::filesystem::is_regular_file(entry) && entry.extension() == ".txt" && stem.size() == 2) {
		language_code code{stem[0], stem[1]};
		tr::localization_map temp;
		temp.load(entry);
		std::string name{temp.contains("language_name") ? temp["language_name"] : stem};
		std::string font{temp.contains("font") ? temp["font"] : std::string{}};
		available_languages.insert({code, {std::move(name), std::move(font)}});
	}
}

static std::map<language_code, language_info> load_languages()
{
	try {
		std::map<language_code, language_info> available_languages;
		for (const auto& entry : std::filesystem::directory_iterator{debug_settings::instance().data_directory() / "localization"}) {
			load_language(available_languages, entry);
		}
		for (const auto& entry : std::filesystem::directory_iterator{debug_settings::instance().user_directory() / "localization"}) {
			load_language(available_languages, entry);
		}
		return available_languages;
	}
	catch (std::exception&) {
		return {};
	}
}

/////////////////////////////////////////////////////////////// LOCALIZATION //////////////////////////////////////////////////////////////

//

localization::localization()
	: available_languages{load_languages()}
{
	reload(active_settings::instance()->language);
}

localization& localization::instance()
{
	static localization instance{};
	return instance;
}

//

bool localization::use_different_fonts(language_code first, language_code second) const
{
	const auto first_it{available_languages.find(first)};
	const auto second_it{available_languages.find(second)};
	return first_it != second_it && (first_it == available_languages.end() || first_it->second.font != second_it->second.font);
}

//

std::string_view localization::operator[](std::string_view tag) const
{
	return m_map[tag];
}

void localization::reload(language_code language)
{
	if (!available_languages.contains(language)) {
		return;
	}

	const std::string_view name{language.data(), 2};
	tr::localization_map old{std::move(m_map)};
	try {
		const std::string filename{TR_FMT::format("localization/{}.txt", name)};
		std::filesystem::path path{debug_settings::instance().data_directory() / filename};
		if (!std::filesystem::exists(path)) {
			path = debug_settings::instance().user_directory() / filename;
		}
		m_map.load(path);
	}
	catch (std::exception&) {
		m_map = std::move(old);
	}
}