#pragma once
#include "global.hpp"

////////////////////////////////////////////////////////////////// TYPES //////////////////////////////////////////////////////////////////

// Language code datatype.
using language_code = std::array<char, 2>;
// Language information.
struct language {
	// The name of the language.
	std::string name;
	// The name of the font file used by the language.
	std::string font;
};

///////////////////////////////////////////////////////////////// ENGINE //////////////////////////////////////////////////////////////////

namespace engine {
	// Global language list.
	inline std::map<language_code, language> languages;
	// Loads the language list.
	void load_languages();

	// Global localization map.
	inline tr::localization_map loc;
	// Loads localization from file.
	void load_localization();
} // namespace engine