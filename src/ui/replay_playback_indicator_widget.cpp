#include "../../include/system.hpp"
#include "../../include/ui/widget.hpp"

//

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

constexpr std::array<glm::vec2, 19> FAST_SPEED_POSITIONS{{
	{0, 0},      {20, 14.5364}, {20, 0},      {48, 24},      {20, 48},      {20, 33.4636}, {0, 48},
	{4, 9.5409}, {24, 24},      {24, 9.5409}, {41.5364, 24}, {24, 38.4591}, {4, 38.4591},  {4, 9.5409},
	{24, 24},    {4, 38.4591},  {24, 9.5409}, {41.5364, 24}, {24, 38.4591},
}};

constexpr std::array<tr::rgba8, 19> FAST_SPEED_COLORS{{
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {192, 192, 192, 192}, {168, 168, 168, 168},
	{144, 144, 144, 144}, {192, 192, 192, 192}, {168, 168, 168, 168}, {144, 144, 144, 144},
}};

constexpr std::array<u16, 48> FAST_SPEED_INDICES{
	0, 1,  7, 1, 7,  8, 1, 8,  9, 1, 9,  2, 2, 3,  9, 3, 10, 9, 3,  10, 4,  10, 11, 4,
	4, 11, 5, 5, 11, 8, 8, 12, 5, 5, 12, 6, 6, 12, 0, 0, 12, 7, 13, 14, 15, 16, 17, 18,
};

//

replay_playback_indicator_widget::replay_playback_indicator_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP, false}
{
}

glm::vec2 replay_playback_indicator_widget::size() const
{
	return {48, 48};
}

void replay_playback_indicator_widget::add_to_renderer()
{
	const glm::vec2 tl{this->tl()};

	if (engine::held_keymods() & tr::sys::keymod::SHIFT) {
		const tr::gfx::color_mesh_ref mesh{
			engine::basic_renderer().new_color_mesh(layer::UI, 9, tr::poly_outline_idx(3) + tr::poly_idx(3))};
		tr::fill_poly_outline_idx(mesh.indices.begin(), 3, mesh.base_index);
		tr::fill_poly_idx(mesh.indices.begin() + tr::poly_outline_idx(3), 3, mesh.base_index + 6);
		std::ranges::copy(SLOW_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_COLORS, mesh.colors.begin());
	}
	else if (engine::held_keymods() & tr::sys::keymod::CTRL) {
		const tr::gfx::color_mesh_ref mesh{engine::basic_renderer().new_color_mesh(layer::UI, 19, FAST_SPEED_INDICES.size())};
		std::ranges::copy(FAST_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(FAST_SPEED_COLORS, mesh.colors.begin());
		std::ranges::copy(FAST_SPEED_INDICES | std::views::transform([=](u16 i) -> u16 { return i + mesh.base_index; }),
						  mesh.indices.begin());
	}
	else {
		const tr::gfx::color_mesh_ref mesh{
			engine::basic_renderer().new_color_mesh(layer::UI, 9, tr::poly_outline_idx(3) + tr::poly_idx(3))};
		tr::fill_poly_outline_idx(mesh.indices.begin(), 3, mesh.base_index);
		tr::fill_poly_idx(mesh.indices.begin() + tr::poly_outline_idx(3), 3, mesh.base_index + 6);
		std::ranges::copy(NORMAL_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_COLORS, mesh.colors.begin());
	}
}