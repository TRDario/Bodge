#include "../include/localization.hpp"
#include "../include/settings.hpp"

namespace engine {
	// Loads a language.
	void load_language(const std::filesystem::path& entry);
} // namespace engine

void engine::load_language(const std::filesystem::path& entry)
{
	const std::string stem{std::filesystem::path{entry}.stem().string()};
	if (std::filesystem::is_regular_file(entry) && entry.extension() == ".txt" && stem.size() == 2) {
		language_code code{stem[0], stem[1]};
		tr::localization_map temp;
		temp.load(entry);
		std::string name{temp.contains("language_name") ? temp["language_name"] : stem};
		std::string font{temp.contains("font") ? temp["font"] : std::string{}};
		languages.insert({code, {std::move(name), std::move(font)}});
	}
}

void engine::load_languages()
{
	try {
		std::ranges::for_each(std::filesystem::directory_iterator{cli_settings.datadir / "localization"}, load_language);
		std::ranges::for_each(std::filesystem::directory_iterator{cli_settings.userdir / "localization"}, load_language);
		LOG(tr::severity::INFO, "Found {} language(s).", languages.size());
	}
	catch (std::exception& err) {
		languages.clear();
		LOG(tr::severity::ERROR, "Failed to load language list.");
		LOG_CONTINUE(err);
	}
}

void engine::load_localization()
{
	if (!languages.contains(settings.language)) {
		LOG(tr::severity::ERROR, "Settings language with code '{}' was not found in the language list.",
			std::string_view{settings.language.data(), 2});
		return;
	}

	const std::string_view name{settings.language.data(), 2};
	tr::localization_map old{std::move(loc)};
	try {
		const std::string filename{std::format("localization/{}.txt", name)};
		std::filesystem::path path{cli_settings.datadir / filename};
		if (!std::filesystem::exists(path)) {
			path = cli_settings.userdir / filename;
		}

		std::vector<std::string> errors{loc.load(path)};
		if (errors.empty()) {
			LOG(tr::severity::INFO, "Loaded {} localization.", languages[settings.language].name);
			LOG_CONTINUE("From: '{}'", path.string());
		}
		else {
			LOG(tr::severity::WARN, "Loaded {} localization with {} errors: ", languages[settings.language].name, errors.size());
			LOG_CONTINUE("From: '{}'", path.string());
			for (const std::string& error : errors) {
				LOG_CONTINUE("{}", error);
			}
		}
	}
	catch (std::exception& err) {
		loc = std::move(old);
		LOG(tr::severity::ERROR, "Failed to load '{}' localization.", name);
		LOG_CONTINUE(err);
	}
}