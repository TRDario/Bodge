///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements settings.hpp.                                                                                                              //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/audio.hpp"
#include "../include/renderer.hpp"
#include "../include/settings.hpp"
#include "../include/state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr u8 SETTINGS_VERSION{2};

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

template <> struct tr::binary_reader<settings> : tr::default_binary_reader<settings> {};
template <> struct tr::binary_writer<settings> : tr::default_binary_writer<settings> {};

u16 max_window_size()
{
	const glm::ivec2 display_size{tr::sys::display_size()};
	return u16(std::min(display_size.x, display_size.y));
}

//

void settings::raw_load_from_file()
{
	const std::filesystem::path path{g_cli_settings.user_directory / "settings.dat"};
	try {
		std::ifstream file{tr::open_file_r(path, std::ios::binary)};
		const std::vector<std::byte> raw{tr::decrypt(tr::flush_binary(file))};
		std::span<const std::byte> data{raw};
		if (tr::binary_read<u8>(data) == SETTINGS_VERSION) {
			tr::binary_read<settings>(data, *this);
		}
	}
	catch (std::exception&) {
		return;
	}
}

void settings::validate()
{
	window_size = std::clamp(window_size, MIN_WINDOW_SIZE, max_window_size());
	msaa = std::clamp(msaa, NO_MSAA, tr::sys::max_msaa());
	mouse_sensitivity = std::clamp(mouse_sensitivity, 50_u8, 200_u8);
	primary_hue = u16(primary_hue % 360);
	secondary_hue = u16(secondary_hue % 360);
	sfx_volume = std::min(sfx_volume, 100_u8);
	music_volume = std::min(music_volume, 100_u8);
}

void settings::load_from_file()
{
	raw_load_from_file();
	validate();
}

void settings::save_to_file() const
{
	try {
		std::ofstream file{tr::open_file_w(g_cli_settings.user_directory / "settings.dat", std::ios::binary)};
		std::ostringstream buffer;
		tr::binary_write(buffer, SETTINGS_VERSION);
		tr::binary_write(buffer, *this);
		const std::vector<std::byte> encrypted{tr::encrypt(tr::range_bytes(buffer.view()), g_rng.generate<u8>())};
		tr::binary_write(file, std::span{encrypted});
	}
	catch (std::exception&) {
		return;
	}
}

//

bool settings::restart_required_to_apply(const settings& new_settings) const
{
	return new_settings.display_mode != display_mode ||
		   (display_mode == display_mode::WINDOWED && new_settings.window_size != window_size) || new_settings.msaa != msaa;
}

bool settings::releasing_graphical_resources_required_to_apply(const settings& new_settings) const
{
	return restart_required_to_apply(new_settings) || !g_languages.contains(language) ||
		   g_languages[language].font != g_languages[new_settings.language].font;
}

void settings::apply(const settings& new_settings)
{
	const bool restart_required{restart_required_to_apply(new_settings)};
	const settings previous{*this};

	*this = new_settings;
	if (restart_required) {
		auto temp{std::move(g_state_machine)};
		g_renderer.reset();
		tr::sys::close_window();
		open_window();
		g_renderer.emplace();
		g_state_machine = std::move(temp);
		tr::sys::show_window();
	}
	else if (vsync != previous.vsync) {
		tr::sys::set_window_vsync(vsync ? tr::sys::vsync::ADAPTIVE : tr::sys::vsync::DISABLED);
	}

	if (previous.language != language) {
		load_localization();
	}
	if (!g_languages.contains(previous.language) || g_languages[previous.language].font != g_languages[g_settings.language].font) {
		g_text_engine.set_language_font();
	}

	g_audio.apply_settings();
}