#include "../include/settings.hpp"
#include "../include/legacy_formats.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings file version identifier.
constexpr u8 SETTINGS_VERSION{1};

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
		const u8 version{tr::binary_read<u8>(data)};
		switch (version) {
		case 0:
			raw_load_v0(data);
			return;
		case 1:
			*this = tr::binary_read<::settings>(data);
			return;
		default:
			return;
		}
	}
	catch (std::exception&) {
		return;
	}
}

void settings::raw_load_v0(std::span<const std::byte> data)
{
	const settings_v0 temp{tr::binary_read<settings_v0>(data)};
	window_size = temp.window_size;
	display_mode = temp.display_mode;
	msaa = temp.msaa;
	primary_hue = temp.primary_hue;
	secondary_hue = temp.secondary_hue;
	sfx_volume = temp.sfx_volume;
	music_volume = temp.music_volume;
	language = temp.language;
}

void settings::validate()
{
	window_size = std::clamp(window_size, MIN_WINDOW_SIZE, max_window_size());
	msaa = std::clamp(msaa, NO_MSAA, tr::sys::max_msaa());
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