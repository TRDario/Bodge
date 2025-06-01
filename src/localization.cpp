#include "../include/localization.hpp"
#include "../include/settings.hpp"

void load_language(const path& entry)
{
	const string stem{path{entry}.stem()};
	if (is_regular_file(entry) && entry.extension() == ".txt" && stem.size() == 2) {
		language_code code{stem[0], stem[1]};
		tr::localization_map localization;
		localization.load(entry);
		string name{localization.contains("language_name") ? localization["language_name"] : stem};
		string font{localization.contains("font") ? localization["font"] : string{}};
		languages.insert({code, {std::move(name), std::move(font)}});
	}
}

void load_languages() noexcept
{
	try {
		rs::for_each(directory_iterator{cli_settings.datadir / "localization"}, load_language);
		rs::for_each(directory_iterator{cli_settings.userdir / "localization"}, load_language);
		LOG(INFO, "Found {} language(s).", languages.size());
	}
	catch (std::exception& err) {
		languages.clear();
		LOG(ERROR, "Failed to load language list: {}", err.what());
	}
}

void load_localization() noexcept
{
	if (!languages.contains(settings.language)) {
		LOG(ERROR, "Settings language with code '{}' was not found in the language list.", string_view{settings.language});
		return;
	}

	const string_view name{settings.language};
	tr::localization_map old{std::move(localization)};
	try {
		string filename{format("localization/{}.txt", name)};
		path path{cli_settings.datadir / filename};
		if (!exists(path)) {
			path = cli_settings.userdir / filename;
		}

		vector<string> errors{localization.load(path)};
		if (errors.empty()) {
			LOG(INFO, "Loaded {} localization from '{}'.", languages[settings.language].name, path.string());
		}
		else {
			LOG(WARN, "Loaded {} localization from '{}' with {} errors: ", languages[settings.language].name, path.string(), errors.size());
			for (string& error : errors) {
				LOG(ERROR, "{}", error);
			}
		}
	}
	catch (std::exception& err) {
		localization = std::move(old);
		LOG(ERROR, "Failed to load '{}' localization: {}", name, err.what());
	}
}