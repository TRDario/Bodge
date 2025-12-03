#include "../../include/graphics.hpp"
#include "../../include/ui/widget.hpp"

///////////////////////////////////////////////////////////// TEXT CALLBACKS //////////////////////////////////////////////////////////////

std::string loc_text_callback::operator()() const
{
	return std::string{g_loc[tag]};
}

std::string tooltip_loc_text_callback::operator()() const
{
	return std::string{tag != nullptr ? g_loc[tag] : std::string{}};
}

std::string string_text_callback::operator()() const
{
	return str;
}

///////////////////////////////////////////////////////////////// WIDGET //////////////////////////////////////////////////////////////////

widget::widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, bool writable)
	: alignment{alignment}, pos{pos}, tooltip_cb{std::move(tooltip_cb)}, m_opacity{0}, m_writable{writable}
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
	m_opacity.change(tween::CUBIC, 0, time);
}

void widget::unhide()
{
	m_opacity = 1;
}

void widget::unhide(ticks time)
{
	m_opacity.change(tween::CUBIC, 1, time);
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
	return m_writable;
}

void widget::update()
{
	pos.update();
	m_opacity.update();
}

/////////////////////////////////////////////////////////////// TEXT WIDGET ///////////////////////////////////////////////////////////////

text_widget::text_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, bool writable,
						 text_callback text_cb, font font, text_style style, float font_size, int max_width)
	: widget{pos, alignment, unhide_time, tooltip_cb, writable}
	, text_cb{text_cb}
	, m_font{font}
	, m_style{style}
	, m_font_size{font_size}
	, m_max_width{max_width}
	, m_last_text{text_cb()}
	, m_cache{g_text_engine.render_text(m_last_text, g_text_engine.determine_font(m_last_text, m_font), m_style, m_font_size,
										m_font_size / 12, m_max_width, tr::halign::CENTER)}
	, m_last_size{tr::get<tr::bitmap>(m_cache).size()}
{
}

glm::vec2 text_widget::size() const
{
	return m_last_size / g_graphics->render_scale();
}

void text_widget::release_graphical_resources()
{
	m_cache = std::monostate{};
}

void text_widget::add_to_renderer_raw(tr::rgba8 tint)
{
	update_cache();

	tint.a *= opacity();

	const tr::gfx::texture& texture{tr::get<tr::gfx::texture>(m_cache)};
	const tr::gfx::simple_textured_mesh_ref quad{g_graphics->basic_renderer.new_textured_fan(layer::UI, 4, texture)};
	tr::fill_rectangle_vertices(quad.positions, {tl(), text_widget::size()});
	tr::fill_rectangle_vertices(quad.uvs, {{}, m_last_size / glm::vec2{texture.size()}});
	std::ranges::fill(quad.tints, tint);
}

void text_widget::update_cache() const
{
	const auto cache_too_small{
		[](const tr::gfx::texture& t, const tr::bitmap& b) { return t.size().x < b.size().x || t.size().y < b.size().y; },
	};

	std::string text{text_cb()};
	if (std::holds_alternative<std::monostate>(m_cache) || m_last_text != text) {
		const tr::bitmap render{g_text_engine.render_text(text, g_text_engine.determine_font(text, m_font), m_style, m_font_size,
														  m_font_size / 12, m_max_width, tr::halign::CENTER)};
		if (!std::holds_alternative<tr::gfx::texture>(m_cache) || cache_too_small(tr::get<tr::gfx::texture>(m_cache), render)) {
			m_cache = tr::gfx::texture{render};
			TR_SET_LABEL(tr::get<tr::gfx::texture>(m_cache), TR_FMT::format("(Bodge) Widget texture"));
		}
		else {
			tr::gfx::texture& texture{tr::get<tr::gfx::texture>(m_cache)};
			texture.clear({});
			texture.set_region({}, render);
		}
		m_last_size = render.size();
		m_last_text = std::move(text);
	}
	else if (std::holds_alternative<tr::bitmap>(m_cache)) {
		const tr::bitmap render{std::move(tr::get<tr::bitmap>(m_cache))};
		m_cache = tr::gfx::texture{render};
		TR_SET_LABEL(tr::get<tr::gfx::texture>(m_cache), TR_FMT::format("(Bodge) Widget texture"));
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