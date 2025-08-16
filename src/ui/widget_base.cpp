#include "../../include/ui/widget_base.hpp"
#include "../../include/graphics.hpp"

/////////////////////////////////////////////////////////////// TEXT CALLBACKS

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

widget::widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, bool writable)
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
	m_opacity.change(interp::CUBIC, 0, time);
}

void widget::unhide()
{
	m_opacity = 1;
}

void widget::unhide(ticks time)
{
	m_opacity.change(interp::CUBIC, 1, time);
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

text_widget::text_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, bool writable,
						 text_callback text_cb, font font, tr::system::ttf_style style, float font_size, int max_width)
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
	if (!m_cached.has_value()) {
		update_cache();
	}
	return m_cached->size / engine::render_scale();
}

void text_widget::release_graphical_resources()
{
	m_cached.reset();
}

void text_widget::add_to_renderer_raw(tr::rgba8 tint)
{
	update_cache();

	tint.a *= opacity();

	const tr::gfx::simple_textured_mesh_ref quad{tr::gfx::renderer_2d::new_textured_fan(layer::UI, 4, m_cached->texture)};
	tr::fill_rect_vtx(quad.positions, {tl(), text_widget::size()});
	tr::fill_rect_vtx(quad.uvs, {{}, m_cached->size / glm::vec2{m_cached->texture.size()}});
	std::ranges::fill(quad.tints, tint);
}

void text_widget::update_cache() const
{
	std::string text{text_cb()};
	if (!m_cached.has_value() || m_cached->text != text) {
		tr::bitmap render{engine::render_text(text, engine::determine_font(text, m_font), m_style, m_font_size, m_font_size / 12,
											  m_max_width, tr::halign::CENTER)};
		if (!m_cached || m_cached->texture.size().x < render.size().x || m_cached->texture.size().y < render.size().y) {
			m_cached.emplace(tr::gfx::texture{render}, render.size(), std::string{text});
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