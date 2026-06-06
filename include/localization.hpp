///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides localization functionality.                                                                                                  //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "global.hpp"

////////////////////////////////////////////////////////////////// TYPES //////////////////////////////////////////////////////////////////

// 2-letter language code used for identification.
using language_code = std::array<char, 2>;
// Language information.
struct language_info {
	// The full name of the language.
	std::string name;
	// The name of the font used for the language.
	std::string font;
};

/////////////////////////////////////////////////////////////// LOCALIZATION //////////////////////////////////////////////////////////////

// Localization manager singleton.
class localization {
  public:
	// Gets the localization manager instance.
	static localization& instance();

	// List of available languages.
	const std::map<language_code, language_info> available_languages;
	// Gets whether two languages use different fonts.
	bool use_different_fonts(language_code first, language_code second) const;

	// Gets a localization value.
	std::string_view operator[](std::string_view tag) const;
	// Reloads the localization.
	void reload(language_code language);

  private:
	// Underlying localization map.
	tr::localization_map m_map;

	// Loads the list of available languages and the initial localization.
	localization();
};