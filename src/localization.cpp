#include "../include/localization.hpp"
#include "../include/settings.hpp"

void load_language(const std::filesystem::path& entry)
{
	const std::string stem{std::filesystem::path{entry}.stem().string()};
	if (std::filesystem::is_regular_file(entry) && entry.extension() == ".txt" && stem.size() == 2) {
		language_code code{stem[0], stem[1]};
		tr::localization_map localization;
		localization.load(entry);
		std::string name{localization.contains("language_name") ? localization["language_name"] : stem};
		std::string font{localization.contains("font") ? localization["font"] : std::string{}};
		languages.insert({code, {std::move(name), std::move(font)}});
	}
}

void load_languages() noexcept
{
	try {
		std::ranges::for_each(std::filesystem::directory_iterator{cli_settings.datadir / "localization"}, load_language);
		std::ranges::for_each(std::filesystem::directory_iterator{cli_settings.userdir / "localization"}, load_language);
		LOG(tr::severity::INFO, "Found {} language(s).", languages.size());
	}
	catch (std::exception& err) {
		languages.clear();
		LOG(tr::severity::ERROR, "Failed to load language list:");
		LOG_CONTINUE("{}", err.what());
	}
}

void load_localization() noexcept
{
	if (!languages.contains(settings.language)) {
		LOG(tr::severity::ERROR, "Settings language with code '{}' was not found in the language list.",
			std::string_view{settings.language.data(), 2});
		return;
	}

	const std::string_view name{settings.language.data(), 2};
	tr::localization_map old{std::move(localization)};
	try {
		const std::string filename{std::format("localization/{}.txt", name)};
		std::filesystem::path path{cli_settings.datadir / filename};
		if (!std::filesystem::exists(path)) {
			path = cli_settings.userdir / filename;
		}

		std::vector<std::string> errors{localization.load(path)};
		if (errors.empty()) {
			LOG(tr::severity::INFO, "Loaded {} localization from '{}'.", languages[settings.language].name, path.string());
		}
		else {
			LOG(tr::severity::WARN, "Loaded {} localization from '{}' with {} errors: ", languages[settings.language].name, path.string(),
				errors.size());
			for (const std::string& error : errors) {
				LOG(tr::severity::ERROR, "{}", error);
			}
		}
	}
	catch (std::exception& err) {
		localization = std::move(old);
		LOG(tr::severity::ERROR, "Failed to load '{}' localization:", name);
		LOG_CONTINUE("{}", err.what());
	}
}