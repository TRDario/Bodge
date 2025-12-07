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

///////////////////////////////////////////////////////////////// GLOBALS /////////////////////////////////////////////////////////////////

// Global list of available languages.
inline std::map<language_code, language_info> g_languages;
// Loads available language information into g_languages.
void load_languages();

// Global localization map.
inline tr::localization_map g_loc;
// Loads localization into g_loc.
void load_localization();