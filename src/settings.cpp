///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements settings.hpp.                                                                                                              //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/settings.hpp"
#include "../include/audio.hpp"
#include "../include/renderer.hpp"
#include "../include/state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr u8 SETTINGS_VERSION{3};

//////////////////////////////////////////////////////////////// SETTINGS /////////////////////////////////////////////////////////////////

template <> struct tr::binary_reader<settings> {
	static std::span<const std::byte> read_from_span(std::span<const std::byte> span, settings& out)
	{
		span = tr::binary_read(span, out.window_size);
		span = tr::binary_read(span, out.display_mode);
		span = tr::binary_read(span, out.vsync);
		span = tr::binary_read(span, out.mouse_sensitivity);
		span = tr::binary_read(span, out.player_skin);
		span = tr::binary_read(span, out.primary_hue);
		span = tr::binary_read(span, out.secondary_hue);
		span = tr::binary_read(span, out.sfx_volume);
		span = tr::binary_read(span, out.music_volume);
		return tr::binary_read(span, out.language);
	}
};

template <> struct tr::binary_writer<settings> {
	static void write_to_stream(std::ostream& os, const settings& in)
	{
		tr::binary_write(os, in.window_size);
		tr::binary_write(os, in.display_mode);
		tr::binary_write(os, in.vsync);
		tr::binary_write(os, in.mouse_sensitivity);
		tr::binary_write(os, in.player_skin);
		tr::binary_write(os, in.primary_hue);
		tr::binary_write(os, in.secondary_hue);
		tr::binary_write(os, in.sfx_volume);
		tr::binary_write(os, in.music_volume);
		tr::binary_write(os, in.language);
	}
};

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
	return new_settings.display_mode != display_mode || (display_mode == display_mode::WINDOWED && new_settings.window_size != window_size);
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