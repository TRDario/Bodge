///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements ui/widget_base.hpp.                                                                                                        //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/renderer.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////// INTERNAL HELPERS /////////////////////////////////////////////////////////////

// Determines whether the cache is too small to fit an image.
static bool cache_too_small(const tr::gfx::texture& cache, const tr::bitmap& image)
{
	return cache.size().x < image.size().x || cache.size().y < image.size().y;
}

////////////////////////////////////////////////////////////// TEXT COMMANDS //////////////////////////////////////////////////////////////

std::string localized_text::operator()() const
{
	return std::string{localization[tag]};
}

std::string constant_text::operator()() const
{
	return str;
}

///////////////////////////////////////////////////////////////// WIDGET //////////////////////////////////////////////////////////////////

widget::widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_command tooltip_text)
	: pos{pos}, tooltip_text{std::move(tooltip_text)}, m_alignment{alignment}, m_opacity{0}
{
	if (unhide_time != DONT_UNHIDE) {
		unhide(unhide_time);
	}
}

glm::vec2 widget::tl() const
{
	return tr::tl(glm::vec2{pos}, size(), m_alignment);
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
	m_opacity.change(0, time);
}

void widget::move_and_hide(glm::vec2 end, ticks time)
{
	pos.move(end, time);
	hide(time);
}

void widget::move_x_and_hide(float end, ticks time)
{
	pos.move_x(end, time);
	hide(time);
}

void widget::move_y_and_hide(float end, ticks time)
{
	pos.move_y(end, time);
	hide(time);
}

void widget::unhide()
{
	m_opacity = 1;
}

void widget::unhide(ticks time)
{
	m_opacity.change(1, time);
}

void widget::move_x_and_unhide(float end, ticks time)
{
	pos.move_x(end, time);
	unhide(time);
}

void widget::move_y_and_unhide(float end, ticks time)
{
	pos.move_y(end, time);
	unhide(time);
}

bool widget::hidden() const
{
	return float{m_opacity} == 0;
}

bool widget::interactible() const
{
	return false;
}

bool widget::writable() const
{
	return false;
}

void widget::tick()
{
	pos.tick();
	m_opacity.tick();
}

/////////////////////////////////////////////////////////////// TEXT WIDGET ///////////////////////////////////////////////////////////////

text_widget::text_widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_command tooltip_text, text_command text,
						 font font, tr::sys::ttf_style style, float font_size, int max_width)
	: widget{pos, alignment, unhide_time, tooltip_text}
	, m_font{font}
	, m_style{style}
	, m_font_size{font_size}
	, m_max_width{max_width}
	, m_text{text}
	, m_last_text{text()}
	, m_cache{renderer::instance().text_engine.render_text(
		  ::text{
			  m_last_text,
			  renderer::instance().text_engine.determine_font(m_last_text, m_font),
			  m_style,
			  m_font_size,
			  m_font_size / 12,
			  float(m_max_width),
		  },
		  tr::halign::CENTER)}
	, m_last_size{tr::get<tr::bitmap>(m_cache).size()}
{
}

glm::vec2 text_widget::size() const
{
	return m_last_size / renderer::instance().scale();
}

void text_widget::release_graphical_resources()
{
	m_cache = std::monostate{};
}

void text_widget::add_to_renderer_raw(renderer& renderer, tr::rgba8 tint)
{
	update_cache(renderer.text_engine);

	tint.a *= opacity();

	const tr::gfx::texture& texture{tr::get<tr::gfx::texture>(m_cache)};
	const tr::gfx::simple_textured_mesh_ref quad{renderer.basic().new_textured_fan(layer::UI, 4, texture)};
	tr::fill_rectangle_vertices(quad.positions, {tl(), text_widget::size()});
	tr::fill_rectangle_vertices(quad.uvs, {{}, m_last_size / glm::vec2{texture.size()}});
	std::ranges::fill(quad.tints, tint);
}

void text_widget::update_cache(text_engine& text_engine) const
{
	std::string text_string{m_text()};
	if (std::holds_alternative<std::monostate>(m_cache) || m_last_text != text_string) {
		const font font{text_engine.determine_font(text_string, m_font)};
		const text text{text_string, font, m_style, m_font_size, m_font_size / 12, float(m_max_width)};
		const tr::bitmap render{text_engine.render_text(text, tr::halign::CENTER)};
		tr::gfx::texture* const cache_texture{std::get_if<tr::gfx::texture>(&m_cache)};
		if (cache_texture == nullptr || cache_too_small(*cache_texture, render)) {
			[[maybe_unused]] tr::gfx::texture& texture{m_cache.emplace<tr::gfx::texture>(render)};
			TR_SET_LABEL(texture, TR_FMT::format("(Bodge) Widget texture"));
		}
		else {
			cache_texture->clear({});
			cache_texture->set_region({}, render);
		}
		m_last_size = render.size();
		m_last_text = std::move(text_string);
	}
	else {
		tr::bitmap* const cache_bitmap{std::get_if<tr::bitmap>(&m_cache)};
		if (cache_bitmap != nullptr) {
			const tr::bitmap source{std::move(*cache_bitmap)};
			[[maybe_unused]] tr::gfx::texture& texture{m_cache.emplace<tr::gfx::texture>(source)};
			TR_SET_LABEL(texture, TR_FMT::format("(Bodge) Widget texture"));
		}
	}
}

/////////////////////////////////////////////////////////// INTERVAL FORMATTER ////////////////////////////////////////////////////////////

void interval_formatter::from_string(ticks& out, std::string_view str)
{
	float temp{out / 1.0_sf};
	std::from_chars(str.data(), str.data() + str.size(), temp);
	out = ticks(temp * 1_s);
}

std::string interval_formatter::to_string(ticks v)
{
	return TR_FMT::format("{:.1f}s", v / 1.0_sf);
}

std::string interval_formatter::to_string(std::string_view str)
{
	return TR_FMT::format("{}s", str);
}