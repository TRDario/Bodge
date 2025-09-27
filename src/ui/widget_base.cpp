#include "../../include/graphics.hpp"
#include "../../include/ui/widget.hpp"

///////////////////////////////////////////////////////////// TEXT CALLBACKS //////////////////////////////////////////////////////////////

std::string loc_text_callback::operator()() const
{
	return std::string{engine::loc[tag]};
}

std::string tooltip_loc_text_callback::operator()() const
{
	return std::string{tag != nullptr ? engine::loc[tag] : std::string{}};
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
						 text_callback text_cb, font font, tr::sys::ttf_style style, float font_size, int max_width)
	: widget{pos, alignment, unhide_time, tooltip_cb, writable}
	, text_cb{text_cb}
	, m_font{font}
	, m_style{style}
	, m_font_size{font_size}
	, m_max_width{max_width}
{
}

glm::vec2 text_widget::size() const
{
	if (!m_cache.has_value()) {
		update_cache();
	}
	return m_cache->size / engine::render_scale();
}

void text_widget::release_graphical_resources()
{
	m_cache.reset();
}

void text_widget::add_to_renderer_raw(tr::rgba8 tint)
{
	update_cache();

	tint.a *= opacity();

	const tr::gfx::simple_textured_mesh_ref quad{engine::basic_renderer().new_textured_fan(layer::UI, 4, m_cache->texture)};
	tr::fill_rect_vtx(quad.positions, {tl(), text_widget::size()});
	tr::fill_rect_vtx(quad.uvs, {{}, m_cache->size / glm::vec2{m_cache->texture.size()}});
	std::ranges::fill(quad.tints, tint);
}

void text_widget::update_cache() const
{
	std::string text{text_cb()};
	if (!m_cache.has_value() || m_cache->text != text) {
		tr::bitmap render{engine::render_text(text, engine::determine_font(text, m_font), m_style, m_font_size, m_font_size / 12,
											  m_max_width, tr::halign::CENTER)};
		if (!m_cache || m_cache->texture.size().x < render.size().x || m_cache->texture.size().y < render.size().y) {
			m_cache.emplace(tr::gfx::texture{render}, render.size(), std::string{text});
			TR_SET_LABEL(m_cache->texture, TR_FMT::format("(Bodge) Widget texture"));
		}
		else {
			m_cache->texture.clear({});
			m_cache->texture.set_region({}, render);
			m_cache->size = render.size();
			m_cache->text = std::move(text);
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