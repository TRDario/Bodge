#include "../../include/audio.hpp"
#include "../../include/engine.hpp"
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

	tr::simple_color_mesh_ref mesh{tr::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{2, 2}, {16, 16}});
	std::ranges::fill(mesh.colors, tr::rgba8{0, 0, 0, tr::norm_cast<std::uint8_t>(opacity)});
	mesh = tr::renderer_2d::new_color_outline(layer::UI, 4);
	tr::fill_rect_outline_vtx(mesh.positions, {pos + glm::vec2{1, 1}, {18, 18}}, 2.0f);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(mesh.positions, pos + glm::vec2{10, 10}, {7, 1}, {14, 2}, 45_deg);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(mesh.positions, pos + glm::vec2{10, 10}, {7, 1}, {14, 2}, -45_deg);
	std::ranges::fill(mesh.colors, color);
}

// Adds an instance of the "Modified ame speed" icon to the renderer.
void add_modified_game_speed_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity)
{
	color = {color.r, color.g, color.b, static_cast<std::uint8_t>(color.a * opacity)};

	tr::simple_color_mesh_ref mesh{tr::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{2, 2}, {16, 16}});
	std::ranges::fill(mesh.colors, tr::rgba8{0, 0, 0, tr::norm_cast<std::uint8_t>(opacity)});
	mesh = tr::renderer_2d::new_color_outline(layer::UI, 4);
	tr::fill_rect_outline_vtx(mesh.positions, {pos + glm::vec2{1, 1}, {18, 18}}, 2);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::renderer_2d::new_color_outline(layer::UI, 8);
	tr::fill_poly_outline_vtx(mesh.positions, 8, {pos + glm::vec2{10, 10}, 7}, 25_deg, 2);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::renderer_2d::new_color_fan(layer::UI, 4);
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{9, 5}, {2, 5}});
	std::ranges::fill(mesh.colors, color);
}

///////////////////////////////////////////////////////////////// WIDGET //////////////////////////////////////////////////////////////////

widget::widget(std::string_view name, glm::vec2 pos, tr::align alignment, bool hoverable, tooltip_callback tooltip_cb, bool writable,
			   std::vector<tr::key_chord>&& shortcuts)
	: name{name}
	, alignment{alignment}
	, pos{pos}
	, tooltip_cb{std::move(tooltip_cb)}
	, opacity_{0}
	, hoverable_{hoverable}
	, writable_{writable}
	, shortcuts{std::move(shortcuts)}
{
}

glm::vec2 widget::tl() const
{
	return tr::tl(glm::vec2{pos}, size(), alignment);
}

float widget::opacity() const
{
	return opacity_;
}

void widget::hide()
{
	opacity_ = 0;
}

void widget::hide(ticks time)
{
	opacity_.change(0, time);
}

void widget::unhide()
{
	opacity_ = 1;
}

void widget::unhide(ticks time)
{
	opacity_.change(1, time);
}

bool widget::hoverable() const
{
	return hoverable_;
}

bool widget::writable() const
{
	return writable_;
}

bool widget::active() const
{
	return false;
}

bool widget::is_shortcut(const tr::key_chord& chord) const
{
	const std::vector<tr::key_chord>::const_iterator it{std::ranges::find(shortcuts, chord)};
	return it != shortcuts.end();
}

void widget::update()
{
	pos.update();
	opacity_.update();
}

/////////////////////////////////////////////////////////////// TEXT_WIDGET ///////////////////////////////////////////////////////////////

text_widget::text_widget(std::string_view name, glm::vec2 pos, tr::align alignment, bool hoverable, tooltip_callback tooltip_cb,
						 bool writable, std::vector<tr::key_chord>&& shortcuts, font font, tr::ttf_style style, tr::halign text_alignment,
						 float font_size, int max_width, tr::rgba8 color, text_callback text_cb)
	: widget{name, pos, alignment, hoverable, std::move(tooltip_cb), writable, std::move(shortcuts)}
	, color{color}
	, text_cb{std::move(text_cb)}
	, font_{font}
	, style{style}
	, text_alignment{text_alignment}
	, font_size{font_size}
	, max_width{max_width}
{
}

text_widget::text_widget(std::string_view name, glm::vec2 pos, tr::align alignment, font font, tr::ttf_style style, float font_size,
						 text_callback text_cb, tr::rgba8 color)
	: text_widget{name, pos,   alignment,          false,     NO_TOOLTIP,          false, {},
				  font, style, tr::halign::CENTER, font_size, tr::UNLIMITED_WIDTH, color, std::move(text_cb)}
{
}

text_widget::text_widget(std::string_view name, glm::vec2 pos, tr::align alignment, std::string_view tooltip_key, font font,
						 tr::ttf_style style, float font_size, text_callback text_cb)
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  [=] { return std::string{localization[tooltip_key]}; },
				  false,
				  {},
				  font,
				  style,
				  tr::halign::CENTER,
				  font_size,
				  tr::UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  std::move(text_cb)}
{
}

glm::vec2 text_widget::size() const
{
	if (!cached.has_value()) {
		update_cache();
	}
	return cached->size / engine::render_scale();
}

void text_widget::update()
{
	widget::update();
	color.update();
}

void text_widget::release_graphical_resources()
{
	cached.reset();
}

void text_widget::add_to_renderer()
{
	update_cache();

	tr::rgba8 color{this->color};
	color.a = static_cast<std::uint8_t>(color.a * opacity());

	const tr::simple_textured_mesh_ref quad{tr::renderer_2d::new_textured_fan(layer::UI, 4, cached->texture)};
	tr::fill_rect_vtx(quad.positions, {tl(), text_widget::size()});
	tr::fill_rect_vtx(quad.uvs, {{}, cached->size / glm::vec2{cached->texture.size()}});
	std::ranges::fill(quad.tints, tr::rgba8{color});
}

void text_widget::update_cache() const
{
	std::string text{text_cb(name)};
	if (!cached.has_value() || cached->text != text) {
		tr::bitmap render{fonts::render_text(text, font_, style, font_size, font_size / 12, max_width, text_alignment)};
		if (!cached || cached->texture.size().x < render.size().x || cached->texture.size().y < render.size().y) {
			cached.emplace(tr::texture{render}, render.size(), std::move(text));
			if (tr::gfx_context::debug()) {
				cached->texture.set_label(std::format("(Bodge) Widget texture - \"{}\"", name));
			}
		}
		else {
			cached->texture.clear({});
			cached->texture.set_region({}, render);
			cached->size = render.size();
			cached->text = std::move(text);
		}
	}
}

////////////////////////////////////////////////////////// CLICKABLE_TEXT_WIDGET //////////////////////////////////////////////////////////

clickable_text_widget::clickable_text_widget(std::string_view name, glm::vec2 pos, tr::align alignment, font font, float font_size,
											 text_callback text_cb, status_callback status_cb, action_callback action_cb,
											 tooltip_callback tooltip_cb, std::vector<tr::key_chord>&& shortcuts, sound sound)
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  std::move(tooltip_cb),
				  false,
				  std::move(shortcuts),
				  font,
				  tr::ttf_style::NORMAL,
				  tr::halign::CENTER,
				  font_size,
				  tr::UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  std::move(text_cb)}
	, status_cb{std::move(status_cb)}
	, action_cb{std::move(action_cb)}
	, override_disabled_color_left{0}
	, sound_{sound}
{
}

void clickable_text_widget::update()
{
	if (override_disabled_color_left > 0) {
		--override_disabled_color_left;
	}
	text_widget::update();
}

void clickable_text_widget::add_to_renderer()
{
	const interpolated_rgba8 real_color{color};
	if (!active() && override_disabled_color_left == 0) {
		color = {80, 80, 80, 160};
	}
	text_widget::add_to_renderer();
	color = real_color;
}

bool clickable_text_widget::active() const
{
	return status_cb();
}

void clickable_text_widget::on_hover()
{
	color.change("FFFFFF"_rgba8, 0.2_s);
	if (active()) {
		audio::play_sound(sound::HOVER, 0.15f, 0.0f, rng.generate(0.9f, 1.1f));
	}
}

void clickable_text_widget::on_unhover()
{
	color.change({160, 160, 160, 160}, 0.2_s);
}

void clickable_text_widget::on_hold_begin()
{
	color = {32, 32, 32, 255};
	audio::play_sound(sound::HOLD, 0.2f, 0.0f, rng.generate(0.9f, 1.1f));
}

void clickable_text_widget::on_hold_transfer_in()
{
	on_hold_begin();
}

void clickable_text_widget::on_hold_transfer_out()
{
	color.change({160, 160, 160, 160}, 0.2_s);
}

void clickable_text_widget::on_hold_end()
{
	color.change("FFFFFF"_rgba8, 0.2_s);
	action_cb();
	audio::play_sound(sound_, 0.5f, 0.0f, rng.generate(0.9f, 1.1f));
}

void clickable_text_widget::on_shortcut()
{
	if (active()) {
		action_cb();
		color = "FFFFFF"_rgba8;
		color.change(active() ? tr::rgba8{160, 160, 160, 160} : tr::rgba8{80, 80, 80, 160}, 0.2_s);
		override_disabled_color_left = 0.2_s;
		audio::play_sound(sound_, 0.5f, 0.0f, rng.generate(0.9f, 1.1f));
	}
}

/////////////////////////////////////////////////////////////// IMAGE_WIDGET //////////////////////////////////////////////////////////////

// Loads an image and returns a fallback texture if loading fails.
tr::bitmap load_image(std::string_view texture)
{
	try {
		const std::filesystem::path path{cli_settings.datadir / "graphics" / std::format("{}.qoi", texture)};
		tr::bitmap image{tr::load_bitmap_file(path)};
		LOG(tr::severity::INFO, "Loaded texture '{}'.", texture);
		LOG_CONTINUE("From: {}", path.string());
		return image;
	}
	catch (tr::bitmap_load_error& err) {
		LOG(tr::severity::ERROR, "Failed to load texture '{}'.", texture);
		LOG_CONTINUE("{}", err.description());
		LOG_CONTINUE("{}", err.details());
		return tr::create_checkerboard({64, 64});
	}
}

image_widget::image_widget(std::string_view name, glm::vec2 pos, tr::align alignment, std::uint16_t* hue_ref)
	: widget{name, pos, alignment, false, NO_TOOLTIP, false, {}}, texture{load_image(name)}, hue_ref{hue_ref}
{
	texture.set_filtering(tr::min_filter::LINEAR, tr::mag_filter::LINEAR);
}

glm::vec2 image_widget::size() const
{
	return glm::vec2{texture.size()} / 2.0f;
}

void image_widget::add_to_renderer()
{
	tr::rgba8 color{255, 255, 255, 255};
	if (hue_ref != nullptr) {
		color = tr::color_cast<tr::rgba8>(tr::hsv{static_cast<float>(*hue_ref), 1, 1});
	}
	color.a = static_cast<std::uint8_t>(color.a * opacity());

	const tr::simple_textured_mesh_ref quad{tr::renderer_2d::new_textured_fan(layer::UI, 4, texture)};
	tr::fill_rect_vtx(quad.positions, {tl(), size()});
	tr::fill_rect_vtx(quad.uvs, {{0, 0}, {1, 1}});
	std::ranges::fill(quad.tints, tr::rgba8{color});
}

/////////////////////////////////////////////////////////// COLOR_PREVIEW_WIDGET //////////////////////////////////////////////////////////

color_preview_widget::color_preview_widget(std::string_view name, glm::vec2 pos, tr::align alignment, std::uint16_t& hue_ref)
	: widget{name, pos, alignment, false, NO_TOOLTIP, false, {}}, hue_ref{hue_ref}
{
}

glm::vec2 color_preview_widget::size() const
{
	return {48, 48};
}

void color_preview_widget::add_to_renderer()
{
	const tr::rgba8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(hue_ref), 1, 1}), tr::norm_cast<std::uint8_t>(opacity())};
	const tr::rgba8 outline_color{static_cast<std::uint8_t>(color.r / 2), static_cast<std::uint8_t>(color.g / 2),
								  static_cast<std::uint8_t>(color.b / 2), static_cast<std::uint8_t>(color.a / 2)};

	const tr::simple_color_mesh_ref outline{tr::renderer_2d::new_color_outline(layer::UI, 4)};
	tr::fill_rect_outline_vtx(outline.positions, {tl() + 2.0f, size() - 4.0f}, 4.0f);
	std::ranges::fill(outline.colors, outline_color);
	const tr::simple_color_mesh_ref fill{tr::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(fill.positions, {tl() + 4.0f, size() - 8.0f});
	std::ranges::fill(fill.colors, color);
}

/////////////////////////////////////////////////////////////// ARROW_WIDGET //////////////////////////////////////////////////////////////

arrow_widget::arrow_widget(std::string_view name, glm::vec2 pos, tr::align alignment, bool right_arrow, status_callback status_cb,
						   action_callback action_cb, std::vector<tr::key_chord>&& chords)
	: widget{name, pos, alignment, true, NO_TOOLTIP, false, std::move(chords)}
	, right{right_arrow}
	, color{{160, 160, 160, 160}}
	, status_cb{std::move(status_cb)}
	, action_cb{std::move(action_cb)}
	, override_disabled_color_left{0}
{
}

glm::vec2 arrow_widget::size() const
{
	return {30, 48};
}

void arrow_widget::add_to_renderer()
{
	tr::rgba8 color{this->color};
	if (!active() && override_disabled_color_left == 0) {
		color = {80, 80, 80, 160};
	}
	color.a *= opacity();

	const glm::vec2 tl{this->tl()};
	const std::array<glm::vec2, 15>& positions{right ? RIGHT_ARROW_POSITIONS : LEFT_ARROW_POSITIONS};
	const tr::color_mesh_ref arrow{tr::renderer_2d::new_color_mesh(layer::UI, 15, tr::poly_outline_idx(5) + tr::poly_idx(5))};
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
	if (override_disabled_color_left > 0) {
		--override_disabled_color_left;
	}
	// Fixes an edge case of being stuck with the disabled color after using a shortcut.
	if (active() && color == tr::rgba8{80, 80, 80, 160}) {
		color = tr::rgba8{160, 160, 160, 160};
	}
	color.update();
	widget::update();
}

bool arrow_widget::active() const
{
	return status_cb();
}

void arrow_widget::on_hover()
{
	color.change("FFFFFF"_rgba8, 0.2_s);
	if (active()) {
		audio::play_sound(sound::HOVER, 0.15f, 0.0f, rng.generate(0.9f, 1.1f));
	}
}

void arrow_widget::on_unhover()
{
	color.change({160, 160, 160, 160}, 0.2_s);
}

void arrow_widget::on_hold_begin()
{
	color = {32, 32, 32, 255};
	audio::play_sound(sound::HOLD, 0.2f, 0.0f, rng.generate(0.9f, 1.1f));
}

void arrow_widget::on_hold_transfer_in()
{
	on_hold_begin();
}

void arrow_widget::on_hold_transfer_out()
{
	color.change({160, 160, 160, 160}, 0.2_s);
}

void arrow_widget::on_hold_end()
{
	color.change("FFFFFF"_rgba8, 0.2_s);
	action_cb();
	audio::play_sound(sound::CONFIRM, 0.5f, 0.0f, rng.generate(0.9f, 1.1f));
}

void arrow_widget::on_shortcut()
{
	if (active()) {
		action_cb();
		color = "FFFFFF"_rgba8;
		color.change(active() ? tr::rgba8{160, 160, 160, 160} : tr::rgba8{80, 80, 80, 160}, 0.2_s);
		override_disabled_color_left = 0.2_s;
		audio::play_sound(sound::CONFIRM, 0.5f, 0.0f, rng.generate(0.9f, 1.1f));
	}
}

////////////////////////////////////////////////////// REPLAY_PLAYBACK_INDICATOR_WIDGET ///////////////////////////////////////////////////

// Creates a replay playback indicator widget.
replay_playback_indicator_widget::replay_playback_indicator_widget(std::string_view name, glm::vec2 pos, tr::align alignment)
	: widget{name, pos, alignment, false, NO_TOOLTIP, false, {}}
{
}

glm::vec2 replay_playback_indicator_widget::size() const
{
	return {48, 48};
}

void replay_playback_indicator_widget::add_to_renderer()
{
	const glm::vec2 tl{this->tl()};

	if (engine::held_keymods() & tr::keymod::SHIFT) {
		const tr::color_mesh_ref mesh{tr::renderer_2d::new_color_mesh(layer::UI, 9, tr::poly_outline_idx(3) + tr::poly_idx(3))};
		tr::fill_poly_outline_idx(mesh.indices.begin(), 3, mesh.base_index);
		tr::fill_poly_idx(mesh.indices.begin() + tr::poly_outline_idx(3), 3, mesh.base_index + 6);
		std::ranges::copy(SLOW_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_COLORS, mesh.colors.begin());
	}
	else if (engine::held_keymods() & tr::keymod::CTRL) {
		const tr::color_mesh_ref mesh{tr::renderer_2d::new_color_mesh(layer::UI, 19, FAST_SPEED_INDICES.size())};
		std::ranges::copy(FAST_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(FAST_SPEED_COLORS, mesh.colors.begin());
		std::ranges::copy(FAST_SPEED_INDICES | std::views::transform([=](std::uint16_t i) -> std::uint16_t { return i + mesh.base_index; }),
						  mesh.indices.begin());
	}
	else {
		const tr::color_mesh_ref mesh{tr::renderer_2d::new_color_mesh(layer::UI, 9, tr::poly_outline_idx(3) + tr::poly_idx(3))};
		tr::fill_poly_outline_idx(mesh.indices.begin(), 3, mesh.base_index);
		tr::fill_poly_idx(mesh.indices.begin() + tr::poly_outline_idx(3), 3, mesh.base_index + 6);
		std::ranges::copy(NORMAL_SPEED_POSITIONS | std::views::transform([=](glm::vec2 p) -> glm::vec2 { return p + tl; }),
						  mesh.positions.begin());
		std::ranges::copy(SLOW_NORMAL_SPEED_COLORS, mesh.colors.begin());
	}
}

////////////////////////////////////////////////////////////// SCORE WIDGET ///////////////////////////////////////////////////////////////

score_widget::score_widget(std::string_view name, glm::vec2 pos, tr::align alignment, std::size_t rank, ::score* score)
	: text_widget{
		  name,
		  pos,
		  alignment,
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
					  str.append(localization["exited_prematurely"]);
				  }
				  if (flags.modified_game_speed) {
					  if (!str.empty()) {
						  str.push_back('\n');
					  }
					  str.append(localization["modified_game_speed"]);
				  }
				  return str;
			  }
		  },
		  false,
		  {},
		  font::LANGUAGE,
		  tr::ttf_style::NORMAL,
		  tr::halign::CENTER,
		  48,
		  tr::UNLIMITED_WIDTH,
		  {160, 160, 160, 160},
		  [this](auto&) {
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
	const interpolated_rgba8 real_color{color};
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
	const interpolated_rgba8 real_color{color};
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