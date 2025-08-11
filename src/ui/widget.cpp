#include "../../include/audio.hpp"
#include "../../include/graphics.hpp"
#include "../../include/system.hpp"
#include "../../include/ui/widget.hpp"

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

std::string loc_text_callback::operator()() const
{
	return std::string{engine::loc[tag]};
}

std::string string_text_callback::operator()() const
{
	return str;
}

///////////////////////////////////////////////////////////////// WIDGET //////////////////////////////////////////////////////////////////

widget::widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, bool hoverable, text_callback tooltip_cb, bool writable)
	: alignment{alignment}, pos{pos}, tooltip_cb{std::move(tooltip_cb)}, m_opacity{0}, m_hoverable{hoverable}, m_writable{writable}
{
	if (unhide_time != DONT_UNHIDE) {
		unhide(unhide_time);
	}
}

glm::vec2 widget::tl() const
{
	return tr::tl(glm::vec2{pos}, size(), alignment);
}

float widget::opacity() const
{
	return m_opacity;
}

void widget::hide()
{
	m_opacity = 0;
}

void widget::hide(ticks time)
{
	m_opacity.change(interp_mode::CUBE, 0, time);
}

void widget::unhide()
{
	m_opacity = 1;
}

void widget::unhide(ticks time)
{
	m_opacity.change(interp_mode::CUBE, 1, time);
}

bool widget::hoverable() const
{
	return m_hoverable;
}

bool widget::writable() const
{
	return m_writable;
}

bool widget::active() const
{
	return false;
}

void widget::update()
{
	pos.update();
	m_opacity.update();
}

/////////////////////////////////////////////////////////////// TEXT_WIDGET ///////////////////////////////////////////////////////////////

text_widget::text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, bool hoverable, text_callback tooltip_cb,
						 bool writable, font font, tr::system::ttf_style style, tr::halign text_alignment, float font_size, int max_width,
						 tr::rgba8 color, text_callback text_cb)
	: widget{pos, alignment, unhide_time, hoverable, std::move(tooltip_cb), writable}
	, color{color}
	, text_cb{std::move(text_cb)}
	, m_font{font}
	, m_style{style}
	, m_text_alignment{text_alignment}
	, m_font_size{font_size}
	, m_max_width{max_width}
{
}

text_widget::text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, font font, tr::system::ttf_style style,
						 float font_size, text_callback text_cb, tr::rgba8 color)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  false,
				  NO_TOOLTIP,
				  false,
				  font,
				  style,
				  tr::halign::CENTER,
				  font_size,
				  tr::system::UNLIMITED_WIDTH,
				  color,
				  std::move(text_cb)}
{
}

text_widget::text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, const char* tooltip_key, font font,
						 tr::system::ttf_style style, float font_size, text_callback text_cb)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  true,
				  string_text_callback{tooltip_key},
				  false,
				  font,
				  style,
				  tr::halign::CENTER,
				  font_size,
				  tr::system::UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  std::move(text_cb)}
{
}

glm::vec2 text_widget::size() const
{
	if (!m_cached.has_value()) {
		update_cache();
	}
	return m_cached->size / engine::render_scale();
}

void text_widget::update()
{
	widget::update();
	color.update();
}

void text_widget::release_graphical_resources()
{
	m_cached.reset();
}

void text_widget::add_to_renderer()
{
	update_cache();

	tr::rgba8 real_color{color};
	real_color.a = static_cast<std::uint8_t>(real_color.a * opacity());

	const tr::gfx::simple_textured_mesh_ref quad{tr::gfx::renderer_2d::new_textured_fan(layer::UI, 4, m_cached->texture)};
	tr::fill_rect_vtx(quad.positions, {tl(), text_widget::size()});
	tr::fill_rect_vtx(quad.uvs, {{}, m_cached->size / glm::vec2{m_cached->texture.size()}});
	std::ranges::fill(quad.tints, tr::rgba8{real_color});
}

void text_widget::update_cache() const
{
	std::string text{text_cb()};
	if (!m_cached.has_value() || m_cached->text != text) {
		tr::bitmap render{engine::render_text(text, engine::determine_font(text, m_font), m_style, m_font_size, m_font_size / 12,
											  m_max_width, m_text_alignment)};
		if (!m_cached || m_cached->texture.size().x < render.size().x || m_cached->texture.size().y < render.size().y) {
			m_cached.emplace(tr::gfx::texture{render}, render.size(), std::move(text));
			if (tr::gfx::debug()) {
				m_cached->texture.set_label(std::format("(Bodge) Widget texture"));
			}
		}
		else {
			m_cached->texture.clear({});
			m_cached->texture.set_region({}, render);
			m_cached->size = render.size();
			m_cached->text = std::move(text);
		}
	}
}

////////////////////////////////////////////////////////// CLICKABLE_TEXT_WIDGET //////////////////////////////////////////////////////////

clickable_text_widget::clickable_text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, font font,
											 float font_size, text_callback text_cb, status_callback status_cb, action_callback action_cb,
											 text_callback tooltip_cb, sound sound)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  true,
				  std::move(tooltip_cb),
				  false,
				  font,
				  tr::system::ttf_style::NORMAL,
				  tr::halign::CENTER,
				  font_size,
				  tr::system::UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  std::move(text_cb)}
	, m_scb{std::move(status_cb)}
	, m_acb{std::move(action_cb)}
	, m_override_disabled_color_left{0}
	, m_sound{sound}
{
}

void clickable_text_widget::update()
{
	if (m_override_disabled_color_left > 0) {
		--m_override_disabled_color_left;
	}
	text_widget::update();
}

void clickable_text_widget::add_to_renderer()
{
	const interpolator<tr::rgba8> real_color{color};
	if (!active() && m_override_disabled_color_left == 0) {
		color = {80, 80, 80, 160};
	}
	text_widget::add_to_renderer();
	color = real_color;
}

bool clickable_text_widget::active() const
{
	return m_scb();
}

void clickable_text_widget::on_hover()
{
	color.change(interp_mode::LERP, "FFFFFF"_rgba8, 0.2_s);
	if (active()) {
		engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
	}
}

void clickable_text_widget::on_unhover()
{
	color.change(interp_mode::LERP, {160, 160, 160, 160}, 0.2_s);
}

void clickable_text_widget::on_hold_begin()
{
	color = {32, 32, 32, 255};
	engine::play_sound(sound::HOLD, 0.2f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

void clickable_text_widget::on_hold_transfer_in()
{
	on_hold_begin();
}

void clickable_text_widget::on_hold_transfer_out()
{
	color.change(interp_mode::LERP, {160, 160, 160, 160}, 0.2_s);
}

void clickable_text_widget::on_hold_end()
{
	color.change(interp_mode::LERP, "FFFFFF"_rgba8, 0.2_s);
	m_acb();
	engine::play_sound(m_sound, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

void clickable_text_widget::on_shortcut()
{
	if (active()) {
		m_acb();
		color = "FFFFFF"_rgba8;
		color.change(interp_mode::LERP, active() ? tr::rgba8{160, 160, 160, 160} : tr::rgba8{80, 80, 80, 160}, 0.2_s);
		m_override_disabled_color_left = 0.2_s;
		engine::play_sound(m_sound, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
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
	: widget{pos, alignment, unhide_time, false, NO_TOOLTIP, false}, m_texture{load_image(file)}, m_hue_ref{hue_ref}, m_priority{priority}
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
	: widget{pos, alignment, unhide_time, false, NO_TOOLTIP, false}, m_hue_ref{hue_ref}
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
	: widget{pos, alignment, unhide_time, true, NO_TOOLTIP, false}
	, m_right{right_arrow}
	, m_color{{160, 160, 160, 160}}
	, m_scb{std::move(status_cb)}
	, m_acb{std::move(action_cb)}
	, m_override_disabled_color_left{0}
{
}

glm::vec2 arrow_widget::size() const
{
	return {30, 48};
}

void arrow_widget::add_to_renderer()
{
	tr::rgba8 color{m_color};
	if (!active() && m_override_disabled_color_left == 0) {
		color = {80, 80, 80, 160};
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
	if (m_override_disabled_color_left > 0) {
		--m_override_disabled_color_left;
	}
	// Fixes an edge case of being stuck with the disabled color after using a shortcut.
	if (active() && m_color == tr::rgba8{80, 80, 80, 160}) {
		m_color = tr::rgba8{160, 160, 160, 160};
	}
	m_color.update();
	widget::update();
}

bool arrow_widget::active() const
{
	return m_scb();
}

void arrow_widget::on_hover()
{
	m_color.change(interp_mode::LERP, "FFFFFF"_rgba8, 0.2_s);
	if (active()) {
		engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
	}
}

void arrow_widget::on_unhover()
{
	m_color.change(interp_mode::LERP, {160, 160, 160, 160}, 0.2_s);
}

void arrow_widget::on_hold_begin()
{
	m_color = {32, 32, 32, 255};
	engine::play_sound(sound::HOLD, 0.2f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

void arrow_widget::on_hold_transfer_in()
{
	on_hold_begin();
}

void arrow_widget::on_hold_transfer_out()
{
	m_color.change(interp_mode::LERP, {160, 160, 160, 160}, 0.2_s);
}

void arrow_widget::on_hold_end()
{
	m_color.change(interp_mode::LERP, "FFFFFF"_rgba8, 0.2_s);
	m_acb();
	engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

void arrow_widget::on_shortcut()
{
	if (active()) {
		m_acb();
		m_color = "FFFFFF"_rgba8;
		m_color.change(interp_mode::LERP, active() ? tr::rgba8{160, 160, 160, 160} : tr::rgba8{80, 80, 80, 160}, 0.2_s);
		m_override_disabled_color_left = 0.2_s;
		engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
	}
}

////////////////////////////////////////////////////// REPLAY_PLAYBACK_INDICATOR_WIDGET ///////////////////////////////////////////////////

// Creates a replay playback indicator widget.
replay_playback_indicator_widget::replay_playback_indicator_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time)
	: widget{pos, alignment, unhide_time, false, NO_TOOLTIP, false}
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
		  true,
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
		  font::LANGUAGE,
		  tr::system::ttf_style::NORMAL,
		  tr::halign::CENTER,
		  48,
		  tr::system::UNLIMITED_WIDTH,
		  {160, 160, 160, 160},
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
	const interpolator<tr::rgba8> real_color{color};
	if (score == nullptr) {
		color = {80, 80, 80, 160};
	}
	text_widget::add_to_renderer();
	color = real_color;

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
			return clickable_text_widget::size() + glm::vec2{0, 20};
		}
	}
	return clickable_text_widget::size();
}

void replay_widget::add_to_renderer()
{
	const interpolator<tr::rgba8> real_color{color};
	if (!it.has_value()) {
		color = {80, 80, 80, 160};
	}
	text_widget::add_to_renderer();
	color = real_color;

	if (it.has_value()) {
		const score_flags flags{(*it)->second.flags};
		const glm::vec2 text_size{clickable_text_widget::size()};
		const tr::rgba8 color{active() ? tr::rgba8{this->color} : "80808080"_rgba8};
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