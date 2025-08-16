#include "../../include/ui/widget.hpp"
#include "../../include/audio.hpp"
#include "../../include/graphics.hpp"
#include "../../include/system.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Positions of the left arrow mesh.
constexpr std::array<glm::vec2, 15> LEFT_ARROW_POSITIONS{{
	{30, 0},
	{16, 0},
	{0, 24},
	{16, 48},
	{30, 48},
	{26, 4},
	{18.30940108, 4},
	{4.61880215, 24},
	{18.30940108, 44},
	{26, 44},
	{26, 4},
	{18.30940108, 4},
	{4.61880215, 24},
	{18.30940108, 44},
	{26, 44},
}};

// Positions of the right arrow mesh.
constexpr std::array<glm::vec2, 15> RIGHT_ARROW_POSITIONS{{
	{0, 0},
	{14, 0},
	{30, 24},
	{14, 48},
	{0, 48},
	{4, 4},
	{11.69059892, 4},
	{25.38119785, 24},
	{11.69059892, 44},
	{4, 44},
	{4, 4},
	{11.69059892, 4},
	{25.38119785, 24},
	{11.69059892, 44},
	{4, 44},
}};

// Colors used by the arrow meshes.
constexpr std::array<tr::rgba8, 15> ARROW_COLORS{{
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{127, 127, 127, 127},
	{255, 255, 255, 255},
	{255, 255, 255, 255},
	{213, 213, 213, 255},
	{192, 192, 192, 255},
	{192, 192, 192, 255},
}};

// Positions of the normal replay speed indicator mesh.
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

// Positions of the slow replay speed indicator mesh.
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

// Colors used by the slow and normal speed replay indicator meshes.
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

// Positions of the fast replay speed indocator mesh.
constexpr std::array<glm::vec2, 19> FAST_SPEED_POSITIONS{{
	{0, 0},      {20, 14.5364}, {20, 0},      {48, 24},      {20, 48},      {20, 33.4636}, {0, 48},
	{4, 9.5409}, {24, 24},      {24, 9.5409}, {41.5364, 24}, {24, 38.4591}, {4, 38.4591},  {4, 9.5409},
	{24, 24},    {4, 38.4591},  {24, 9.5409}, {41.5364, 24}, {24, 38.4591},
}};

// Colors used by the fast replay speed indicator mesh.
constexpr std::array<tr::rgba8, 19> FAST_SPEED_COLORS{{
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},
	{95, 95, 95, 95},     {95, 95, 95, 95},     {95, 95, 95, 95},     {192, 192, 192, 192}, {168, 168, 168, 168},
	{144, 144, 144, 144}, {192, 192, 192, 192}, {168, 168, 168, 168}, {144, 144, 144, 144},
}};

// Indices of the fast replay speed indicator mesh.
constexpr std::array<std::uint16_t, 48> FAST_SPEED_INDICES{
	0, 1,  7, 1, 7,  8, 1, 8,  9, 1, 9,  2, 2, 3,  9, 3, 10, 9, 3,  10, 4,  10, 11, 4,
	4, 11, 5, 5, 11, 8, 8, 12, 5, 5, 12, 6, 6, 12, 0, 0, 12, 7, 13, 14, 15, 16, 17, 18,
};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

// Adds an instance of the "Exited prematurely" icon to the renderer.
void add_exited_prematurely_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity)
{
	color = {color.r, color.g, color.b, static_cast<std::uint8_t>(color.a * opacity)};

	tr::gfx::simple_color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{2, 2}, {16, 16}});
	std::ranges::fill(mesh.colors, tr::rgba8{0, 0, 0, tr::norm_cast<std::uint8_t>(opacity)});
	mesh = tr::gfx::renderer_2d::new_color_outline(layer::UI, 4);
	tr::fill_rect_outline_vtx(mesh.positions, {pos + glm::vec2{1, 1}, {18, 18}}, 2.0f);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::gfx::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(mesh.positions, pos + glm::vec2{10, 10}, {7, 1}, {14, 2}, 45_deg);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::gfx::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(mesh.positions, pos + glm::vec2{10, 10}, {7, 1}, {14, 2}, -45_deg);
	std::ranges::fill(mesh.colors, color);
}

// Adds an instance of the "Modified ame speed" icon to the renderer.
void add_modified_game_speed_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity)
{
	color = {color.r, color.g, color.b, static_cast<std::uint8_t>(color.a * opacity)};

	tr::gfx::simple_color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{2, 2}, {16, 16}});
	std::ranges::fill(mesh.colors, tr::rgba8{0, 0, 0, tr::norm_cast<std::uint8_t>(opacity)});
	mesh = tr::gfx::renderer_2d::new_color_outline(layer::UI, 4);
	tr::fill_rect_outline_vtx(mesh.positions, {pos + glm::vec2{1, 1}, {18, 18}}, 2);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::gfx::renderer_2d::new_color_outline(layer::UI, 8);
	tr::fill_poly_outline_vtx(mesh.positions, 8, {pos + glm::vec2{10, 10}, 7}, 25_deg, 2);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::gfx::renderer_2d::new_color_fan(layer::UI, 4);
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{9, 5}, {2, 5}});
	std::ranges::fill(mesh.colors, color);
}

////////////////////////////////////////////////////////////// LABEL_WIDGET ///////////////////////////////////////////////////////////////

label_widget::label_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb,
						   text_callback text_cb, tr::system::ttf_style style, float font_size, tr::rgba8 color)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  std::move(tooltip_cb),
				  false,
				  std::move(text_cb),
				  font::LANGUAGE,
				  style,
				  font_size,
				  tr::system::UNLIMITED_WIDTH}
	, color{color}
{
}

void label_widget::update()
{
	widget::update();
	color.update();
}

void label_widget::add_to_renderer()
{
	add_to_renderer_raw(color);
}

/////////////////////////////////////////////////////////// TEXT BUTTON WIDGET ////////////////////////////////////////////////////////////

text_button_widget::text_button_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb,
									   text_callback text_cb, font font, float font_size, status_callback status_cb,
									   action_callback action_cb, sound sound)
	: text_widget{pos,         alignment,
				  unhide_time, std::move(tooltip_cb),
				  false,       std::move(text_cb),
				  font,        tr::system::ttf_style::NORMAL,
				  font_size,   tr::system::UNLIMITED_WIDTH}
	, m_scb{std::move(status_cb)}
	, m_acb{std::move(action_cb)}
	, m_sound{sound}
	, m_interp{m_scb() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
	, m_action_left{0}
{
}

void text_button_widget::update()
{
	text_widget::update();
	m_interp.update();

	if (interactible()) {
		if (!m_held && !m_hovered && !m_selected && m_interp.done() && m_action_left == 0 && m_interp != "A0A0A0A0"_rgba8) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
		else if (m_interp.done() && (m_hovered || m_selected) && !m_held && m_action_left == 0) {
			m_interp.change(interp::CYCLE, tr::color_cast<tr::rgba8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 0.2f, 1.0f}),
							4_s);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if (m_interp.done() && m_action_left == 0 && m_interp != "505050A0"_rgba8) {
			m_interp.change(interp::LERP, "505050A0"_rgba8, 0.1_s);
		}
	}

	if (m_action_left > 0) {
		--m_action_left;
	}
}

void text_button_widget::add_to_renderer()
{
	if (m_action_left > 0) {
		text_widget::add_to_renderer_raw(m_action_left % 0.12_s >= 0.06_s ? "FFFFFF"_rgba8 : "505050A0"_rgba8);
	}
	else {
		text_widget::add_to_renderer_raw(m_interp);
	}
}

bool text_button_widget::interactible() const
{
	return m_scb();
}

void text_button_widget::on_action()
{
	m_acb();
	m_action_left = 0.36_s;
	m_interp = "FFFFFF"_rgba8;
	engine::play_sound(m_sound, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

void text_button_widget::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
			engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

void text_button_widget::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected && m_action_left == 0) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

void text_button_widget::on_held()
{
	if (interactible()) {
		m_held = true;
		if (m_action_left == 0) {
			m_interp = "202020"_rgba8;
		}
	}
}

void text_button_widget::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

void text_button_widget::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
		}
	}
}

void text_button_widget::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered && m_action_left == 0) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

/////////////////////////////////////////////////////////////// IMAGE_WIDGET //////////////////////////////////////////////////////////////

// Loads an image and returns a fallback texture if loading fails.
tr::bitmap load_image(std::string_view texture)
{
	try {
		const std::filesystem::path path{engine::cli_settings.datadir / "graphics" / std::format("{}.qoi", texture)};
		tr::bitmap image{tr::load_bitmap_file(path)};
		LOG(tr::severity::INFO, "Loaded texture '{}'.", texture);
		LOG_CONTINUE("From: '{}'", path.string());
		return image;
	}
	catch (tr::bitmap_load_error& err) {
		LOG(tr::severity::ERROR, "Failed to load texture '{}'.", texture);
		LOG_CONTINUE("{}", err.description());
		LOG_CONTINUE("{}", err.details());
		return tr::create_checkerboard({64, 64});
	}
}

image_widget::image_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, int priority, std::string_view file,
						   std::uint16_t* hue_ref)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP, false}, m_texture{load_image(file)}, m_hue_ref{hue_ref}, m_priority{priority}
{
	m_texture.set_filtering(tr::gfx::min_filter::LINEAR, tr::gfx::mag_filter::LINEAR);
}

glm::vec2 image_widget::size() const
{
	return glm::vec2{m_texture.size()} / 2.0f;
}

void image_widget::add_to_renderer()
{
	tr::rgba8 color{255, 255, 255, 255};
	if (m_hue_ref != nullptr) {
		color = tr::color_cast<tr::rgba8>(tr::hsv{static_cast<float>(*m_hue_ref), 1, 1});
	}
	color.a = static_cast<std::uint8_t>(color.a * opacity());

	const tr::gfx::simple_textured_mesh_ref quad{tr::gfx::renderer_2d::new_textured_fan(layer::UI + m_priority, 4, m_texture)};
	tr::fill_rect_vtx(quad.positions, {tl(), size()});
	tr::fill_rect_vtx(quad.uvs, {{0, 0}, {1, 1}});
	std::ranges::fill(quad.tints, tr::rgba8{color});
}

/////////////////////////////////////////////////////////// COLOR_PREVIEW_WIDGET //////////////////////////////////////////////////////////

color_preview_widget::color_preview_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, std::uint16_t& hue_ref)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP, false}, m_hue_ref{hue_ref}
{
}

glm::vec2 color_preview_widget::size() const
{
	return {48, 48};
}

void color_preview_widget::add_to_renderer()
{
	const tr::rgba8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(m_hue_ref), 1, 1}), tr::norm_cast<std::uint8_t>(opacity())};
	const tr::rgba8 outline_color{static_cast<std::uint8_t>(color.r / 2), static_cast<std::uint8_t>(color.g / 2),
								  static_cast<std::uint8_t>(color.b / 2), static_cast<std::uint8_t>(color.a / 2)};

	const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::UI, 4)};
	tr::fill_rect_outline_vtx(outline.positions, {tl() + 2.0f, size() - 4.0f}, 4.0f);
	std::ranges::fill(outline.colors, outline_color);
	const tr::gfx::simple_color_mesh_ref fill{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(fill.positions, {tl() + 4.0f, size() - 8.0f});
	std::ranges::fill(fill.colors, color);
}

/////////////////////////////////////////////////////////////// ARROW_WIDGET //////////////////////////////////////////////////////////////

arrow_widget::arrow_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, bool right_arrow, status_callback status_cb,
						   action_callback action_cb)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP, false}
	, m_scb{std::move(status_cb)}
	, m_acb{std::move(action_cb)}
	, m_interp{m_scb() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8}
	, m_right{right_arrow}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
	, m_action_left{0}
{
}

glm::vec2 arrow_widget::size() const
{
	return {30, 48};
}

void arrow_widget::add_to_renderer()
{
	tr::rgba8 color;
	if (m_action_left > 0) {
		color = m_action_left % 0.12_s >= 0.06_s ? "FFFFFF"_rgba8 : "505050A0"_rgba8;
	}
	else {
		color = m_interp;
	}
	color.a *= opacity();

	const glm::vec2 tl{this->tl()};
	const std::array<glm::vec2, 15>& positions{m_right ? RIGHT_ARROW_POSITIONS : LEFT_ARROW_POSITIONS};
	const tr::gfx::color_mesh_ref arrow{tr::gfx::renderer_2d::new_color_mesh(layer::UI, 15, tr::poly_outline_idx(5) + tr::poly_idx(5))};
	tr::fill_poly_outline_idx(arrow.indices.begin(), 5, arrow.base_index);
	tr::fill_poly_idx(arrow.indices.begin() + tr::poly_outline_idx(5), 5, arrow.base_index + 10);
	std::ranges::copy(positions | std::views::transform([&](glm::vec2 p) { return p + tl; }), arrow.positions.begin());
	std::ranges::copy(ARROW_COLORS | std::views::transform([&](tr::rgba8 c) -> tr::rgba8 {
						  return {static_cast<std::uint8_t>(c.r * tr::norm_cast<float>(color.r)),
								  static_cast<std::uint8_t>(c.g * tr::norm_cast<float>(color.g)),
								  static_cast<std::uint8_t>(c.b * tr::norm_cast<float>(color.b)),
								  static_cast<std::uint8_t>(c.a * tr::norm_cast<float>(color.a))};
					  }),
					  arrow.colors.begin());
}

void arrow_widget::update()
{
	widget::update();
	m_interp.update();

	if (interactible()) {
		if (!m_held && !m_hovered && !m_selected && m_interp.done() && m_action_left == 0 && m_interp != "A0A0A0A0"_rgba8) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
		else if (m_interp.done() && (m_hovered || m_selected) && !m_held && m_action_left == 0) {
			m_interp.change(interp::CYCLE, tr::color_cast<tr::rgba8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 0.2f, 1.0f}),
							4_s);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if (m_interp.done() && m_action_left == 0 && m_interp != "505050A0"_rgba8) {
			m_interp.change(interp::LERP, "505050A0"_rgba8, 0.1_s);
		}
	}

	if (m_action_left > 0) {
		--m_action_left;
	}
}

bool arrow_widget::interactible() const
{
	return m_scb();
}

void arrow_widget::on_action()
{
	m_acb();
	m_action_left = 0.36_s;
	m_interp = "FFFFFF"_rgba8;
	engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

void arrow_widget::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
			engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

void arrow_widget::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected && m_action_left == 0) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

void arrow_widget::on_held()
{
	if (interactible()) {
		m_held = true;
		if (m_action_left == 0) {
			m_interp = "202020"_rgba8;
		}
	}
}

void arrow_widget::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

void arrow_widget::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
		}
	}
}

void arrow_widget::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered && m_action_left == 0) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

////////////////////////////////////////////////////// REPLAY_PLAYBACK_INDICATOR_WIDGET ///////////////////////////////////////////////////

// Creates a replay playback indicator widget.
replay_playback_indicator_widget::replay_playback_indicator_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time)
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

	if (engine::held_keymods() & tr::system::keymod::SHIFT) {
		const tr::gfx::color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_mesh(layer::UI, 9, tr::poly_outline_idx(3) + tr::poly_idx(3))};
		tr::fill_poly_outline_idx(mesh.indices.begin(), 3, mesh.base_index);
		tr::fill_poly_idx(mesh.indices.begin() + tr::poly_outline_idx(3), 3, mesh.base_index + 6);
		std::ranges::copy(SLOW_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_COLORS, mesh.colors.begin());
	}
	else if (engine::held_keymods() & tr::system::keymod::CTRL) {
		const tr::gfx::color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_mesh(layer::UI, 19, FAST_SPEED_INDICES.size())};
		std::ranges::copy(FAST_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(FAST_SPEED_COLORS, mesh.colors.begin());
		std::ranges::copy(FAST_SPEED_INDICES | std::views::transform([=](std::uint16_t i) -> std::uint16_t { return i + mesh.base_index; }),
						  mesh.indices.begin());
	}
	else {
		const tr::gfx::color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_mesh(layer::UI, 9, tr::poly_outline_idx(3) + tr::poly_idx(3))};
		tr::fill_poly_outline_idx(mesh.indices.begin(), 3, mesh.base_index);
		tr::fill_poly_idx(mesh.indices.begin() + tr::poly_outline_idx(3), 3, mesh.base_index + 6);
		std::ranges::copy(NORMAL_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_COLORS, mesh.colors.begin());
	}
}

////////////////////////////////////////////////////////////// SCORE WIDGET ///////////////////////////////////////////////////////////////

score_widget::score_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, std::size_t rank, ::score* score)
	: text_widget{
		  pos,
		  alignment,
		  unhide_time,
		  [this] {
			  if (this->score == nullptr) {
				  return std::string{};
			  }
			  else {
				  const score_flags flags{this->score->flags};
				  std::string str{this->score->description};
				  if ((flags.exited_prematurely || flags.modified_game_speed) && !str.empty()) {
					  str.push_back('\n');
				  }
				  if (flags.exited_prematurely) {
					  if (!str.empty()) {
						  str.push_back('\n');
					  }
					  str.append(engine::loc["exited_prematurely"]);
				  }
				  if (flags.modified_game_speed) {
					  if (!str.empty()) {
						  str.push_back('\n');
					  }
					  str.append(engine::loc["modified_game_speed"]);
				  }
				  return str;
			  }
		  },
		  false,
		  [this] {
			  if (this->score == nullptr) {
				  return std::string{"----------------------------------"};
			  }

			  const ticks result{this->score->result};
			  const ticks result_m{result / 60_s};
			  const ticks result_s{(result % 60_s) / 1_s};
			  const ticks result_ms{(result % 1_s) * 100 / 1_s};
			  const std::chrono::system_clock::time_point utc_tp{std::chrono::seconds{this->score->timestamp}};
			  const auto tp{std::chrono::current_zone()->std::chrono::time_zone::to_local(utc_tp)};
			  const std::chrono::hh_mm_ss hhmmss{tp - std::chrono::floor<std::chrono::days>(tp)};
			  const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(tp)};
			  const int year{ymd.year()};
			  const unsigned int month{ymd.month()};
			  const unsigned int day{ymd.day()};
			  const auto hour{hhmmss.hours().count()};
			  const auto minute{hhmmss.minutes().count()};
			  if (this->rank == DONT_SHOW_RANK) {
				  return std::format("{}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", result_m, result_s, result_ms, year, month, day, hour,
									 minute);
			  }
			  else {
				  return std::format("{}) {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", this->rank, result_m, result_s, result_ms, year,
									 month, day, hour, minute);
			  }
		  },
		  font::LANGUAGE,
		  tr::system::ttf_style::NORMAL,
		  48,
		  tr::system::UNLIMITED_WIDTH,
	  }
	, rank{rank}
	, score{score}
{
}

glm::vec2 score_widget::size() const
{
	if (score != nullptr) {
		const auto icons{score->flags.exited_prematurely + score->flags.modified_game_speed};
		if (icons != 0) {
			return text_widget::size() + glm::vec2{0, 20};
		}
	}
	return text_widget::size();
}

void score_widget::add_to_renderer()
{
	const tr::rgba8 color{score != nullptr ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8};
	text_widget::add_to_renderer_raw(color);
	if (score != nullptr) {
		const score_flags flags{score->flags};
		const glm::vec2 text_size{text_widget::size()};
		const auto icons{flags.exited_prematurely + flags.modified_game_speed};
		int i = 0;

		if (flags.exited_prematurely) {
			add_exited_prematurely_icon_to_renderer(tl() + glm::vec2{text_size.x / 2 - 15 * icons + 30 * i, text_size.y}, color, opacity());
			++i;
		}
		if (flags.modified_game_speed) {
			add_modified_game_speed_icon_to_renderer(tl() + glm::vec2{text_size.x / 2 - 15 * icons + 30 * i, text_size.y}, color,
													 opacity());
			++i;
		}
	}
}

////////////////////////////////////////////////////////////// REPLAY_WIDGET //////////////////////////////////////////////////////////////

glm::vec2 replay_widget::size() const
{
	if (it.has_value()) {
		const score_flags flags{(*it)->second.flags};
		const auto icons{flags.exited_prematurely + flags.modified_game_speed};
		if (icons != 0) {
			return text_button_widget::size() + glm::vec2{0, 20};
		}
	}
	return text_button_widget::size();
}

void replay_widget::add_to_renderer()
{
	if (!it.has_value()) {
		text_widget::add_to_renderer_raw("505050A0"_rgba8);
	}
	else {
		text_button_widget::add_to_renderer();
	}

	if (it.has_value()) {
		const score_flags flags{(*it)->second.flags};
		const glm::vec2 text_size{text_button_widget::size()};
		const tr::rgba8 color{interactible() ? tr::rgba8{m_interp} : "80808080"_rgba8};
		const auto icons{flags.exited_prematurely + flags.modified_game_speed};
		int i = 0;

		if (flags.exited_prematurely) {
			add_exited_prematurely_icon_to_renderer(tl() + glm::vec2{text_size.x / 2 - 15 * icons + 30 * i, text_size.y}, color, opacity());
			++i;
		}
		if (flags.modified_game_speed) {
			add_modified_game_speed_icon_to_renderer(tl() + glm::vec2{text_size.x / 2 - 15 * icons + 30 * i, text_size.y}, color,
													 opacity());
			++i;
		}
	}
}