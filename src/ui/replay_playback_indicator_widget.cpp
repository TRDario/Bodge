#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Position data for the normal speed icon.
constexpr std::array<glm::vec2, 9> NORMAL_SPEED_POSITIONS{{
	{12, 0},
	{48, 24},
	{12, 48},
	{16, 7.47406836},
	{40.78889744, 24},
	{16, 40.52593164},
	{16, 7.47406836},
	{40.78889744, 24},
	{16, 40.52593164},
}};

// Position data for the slow speed icon.
constexpr std::array<glm::vec2, 9> SLOW_SPEED_POSITIONS{{
	{48, 0},
	{12, 24},
	{48, 48},
	{44, 7.47406836},
	{19.21110256, 24},
	{44, 40.52593164},
	{44, 7.47406836},
	{19.21110256, 24},
	{44, 40.52593164},
}};

// Color data for the slow and normal speed icons.
constexpr std::array<tr::rgba8, 9> SLOW_NORMAL_SPEED_COLORS{{
	{95, 95, 95, 95},
	{95, 95, 95, 95},
	{95, 95, 95, 95},
	{95, 95, 95, 95},
	{95, 95, 95, 95},
	{95, 95, 95, 95},
	{192, 192, 192, 192},
	{168, 168, 168, 192},
	{144, 144, 144, 192},
}};

// Index data for the slow and normal speed icons.
constexpr std::array<u16, tr::polygon_outline_indices(3) + tr::polygon_indices(3)> SLOW_NORMAL_SPEED_INDICES{[] {
	std::array<u16, tr::polygon_outline_indices(3) + tr::polygon_indices(3)> arr{};
	auto arr_it{arr.begin()};
	arr_it = tr::fill_convex_polygon_outline_indices(arr_it, 3, 0);
	arr_it = tr::fill_convex_polygon_indices(arr_it, 3, 6);
	return arr;
}()};

// Position data for the fast speed icon.
constexpr std::array<glm::vec2, 19> FAST_SPEED_POSITIONS{{
	{0, 0},      {20, 14.5364}, {20, 0},      {48, 24},      {20, 48},      {20, 33.4636}, {0, 48},
	{4, 9.5409}, {24, 24},      {24, 9.5409}, {41.5364, 24}, {24, 38.4591}, {4, 38.4591},  {4, 9.5409},
	{24, 24},    {4, 38.4591},  {24, 9.5409}, {41.5364, 24}, {24, 38.4591},
}};

// Color data for the fast speed icon.
constexpr std::array<tr::rgba8, 19> FAST_SPEED_COLORS{{
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {192, 192, 192, 192}, {168, 168, 168, 168},
	{144, 144, 144, 144}, {192, 192, 192, 192}, {168, 168, 168, 168}, {144, 144, 144, 144},
}};

// Index data for the fast speed icon.
constexpr std::array<u16, 48> FAST_SPEED_INDICES{
	0, 1,  7, 1, 7,  8, 1, 8,  9, 1, 9,  2, 2, 3,  9, 3, 10, 9, 3,  10, 4,  10, 11, 4,
	4, 11, 5, 5, 11, 8, 8, 12, 5, 5, 12, 6, 6, 12, 0, 0, 12, 7, 13, 14, 15, 16, 17, 18,
};

///////////////////////////////////////////////////// REPLAY PLAYBACK INDICATOR WIDGET ////////////////////////////////////////////////////

replay_playback_indicator_widget::replay_playback_indicator_widget(tweened_position pos, tr::align alignment, ticks unhide_time)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP}
{
}

glm::vec2 replay_playback_indicator_widget::size() const
{
	return {48, 48};
}

void replay_playback_indicator_widget::add_to_renderer()
{
	const glm::vec2 tl{this->tl()};
	const auto shift_positions{[=](glm::vec2 p) -> glm::vec2 { return p + tl; }};
	tr::gfx::color_mesh_ref mesh{};
	const auto shift_indices{[&](u16 i) -> u16 { return i + mesh.base_index; }};

	if (g_held_keymods & tr::sys::keymod::SHIFT) {
		mesh = g_renderer->basic.new_color_mesh(layer::UI, 9, SLOW_NORMAL_SPEED_INDICES.size());
		std::ranges::copy(SLOW_SPEED_POSITIONS | std::views::transform(shift_positions), mesh.positions.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_COLORS, mesh.colors.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_INDICES | std::views::transform(shift_indices), mesh.indices.begin());
	}
	else if (g_held_keymods & tr::sys::keymod::CTRL) {
		mesh = g_renderer->basic.new_color_mesh(layer::UI, 19, FAST_SPEED_INDICES.size());
		std::ranges::copy(FAST_SPEED_POSITIONS | std::views::transform(shift_positions), mesh.positions.begin());
		std::ranges::copy(FAST_SPEED_COLORS, mesh.colors.begin());
		std::ranges::copy(FAST_SPEED_INDICES | std::views::transform(shift_indices), mesh.indices.begin());
	}
	else {
		mesh = g_renderer->basic.new_color_mesh(layer::UI, 9, SLOW_NORMAL_SPEED_INDICES.size());
		std::ranges::copy(NORMAL_SPEED_POSITIONS | std::views::transform(shift_positions), mesh.positions.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_COLORS, mesh.colors.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_INDICES | std::views::transform(shift_indices), mesh.indices.begin());
	}
}