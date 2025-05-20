#include "../../include/engine.hpp"
#include "../../include/ui/widget.hpp"

///////////////////////////////////////////////////////////////// WIDGET //////////////////////////////////////////////////////////////////

widget::widget(std::string&& name, glm::vec2 pos, tr::align alignment) noexcept
	: name{std::move(name)}, pos{pos}, align{alignment}, _opacity{0}
{
}

glm::vec2 widget::tl() const noexcept
{
	return tr::tl(glm::vec2{pos}, size(), align);
}

float widget::opacity() const noexcept
{
	return _opacity;
}

void widget::hide() noexcept
{
	_opacity = 0;
}

void widget::hide(std::uint16_t time) noexcept
{
	_opacity.change(0, time);
}

void widget::unhide() noexcept
{
	_opacity = 1;
}

void widget::unhide(std::uint16_t time) noexcept
{
	_opacity.change(1, time);
}

void widget::update()
{
	pos.update();
	_opacity.update();
}

void widget::release_graphical_resources() noexcept {}

//////////////////////////////////////////////////////////////// MOUSABLE /////////////////////////////////////////////////////////////////

mousable::mousable(tooltip_callback tooltip_cb) noexcept
	: tooltip_cb{std::move(tooltip_cb)}
{
}

////////////////////////////////////////////////////////////// SHORTCUTABLE ///////////////////////////////////////////////////////////////

shortcutable::shortcutable(tr::keycode chord_key, tr::keymods chord_mods) noexcept
	: chord_key{chord_key}, chord_mods{chord_mods}
{
}

//////////////////////////////////////////////////////////// BASIC_TEXT_WIDGET ////////////////////////////////////////////////////////////

basic_text_widget::basic_text_widget(std::string&& name, glm::vec2 pos, tr::align alignment, tr::rgba8 color, font font, float font_size,
									 float outline, float max_width, tr::halign text_alignment, text_callback text_cb) noexcept
	: widget{std::move(name), pos, alignment}
	, color{color}
	, _font{font}
	, _font_size{font_size}
	, _outline{outline}
	, _max_width{max_width}
	, _text_alignment{text_alignment}
	, _text_cb{std::move(text_cb)}
{
	update_cache();
}

basic_text_widget::basic_text_widget(std::string&& name, glm::vec2 pos, tr::align alignment, float font_size) noexcept
	: basic_text_widget{std::move(name), pos, alignment, {192, 192, 192, 192}, font::LANGUAGE, font_size, font_size / 12}
{
}

glm::vec2 basic_text_widget::size() const noexcept
{
	return _cached.has_value() ? _cached->size / engine::render_scale() : glm::vec2{0};
}

void basic_text_widget::update() noexcept
{
	widget::update();
	color.update();
}

void basic_text_widget::release_graphical_resources() noexcept
{
	_cached.reset();
}

void basic_text_widget::add_to_renderer()
{
	update_cache();

	tr::rgba8 color{this->color};
	color.a = static_cast<std::uint8_t>(color.a * opacity());

	std::array<tr::tintvtx2, 4> quad;
	tr::fill_rect_vtx(tr::positions(quad), {tl(), size()});
	tr::fill_rect_vtx(tr::uvs(quad), {{}, _cached->size / glm::vec2{_cached->texture.size()}});
	std::ranges::fill(tr::colors(quad), tr::rgba8{color});
	engine::batched_renderer().add_tex_quad(quad, 0, _cached->texture, TRANSFORM);
}

void basic_text_widget::update_cache()
{
	std::string text{_text_cb(name)};
	if (!_cached.has_value() || _cached->text != text) {
		const tr::bitmap render{font_manager.render_text(text, _font, _font_size, _outline, _max_width, _text_alignment)};
		if (!_cached || _cached->texture.size().x < render.size().x || _cached->texture.size().y < render.size().y) {
			_cached.emplace(tr::texture{render}, render.size(), std::move(text));
			if (cli_settings.debug_mode) {
				_cached->texture.set_label(std::format("(Bodge) Widget texture - \"{}\"", name));
			}
		}
		else {
			_cached->texture.clear({});
			_cached->texture.set_region({}, render);
			_cached->size = render.size();
			_cached->text = std::move(text);
		}
	}
}

//////////////////////////////////////////////////////// TOOLTIPPABLE_TEXT_WIDGET /////////////////////////////////////////////////////////

tooltippable_text_widget::tooltippable_text_widget(std::string&& name, glm::vec2 pos, tr::align alignment, std::string_view tooltip_key,
												   float font_size) noexcept
	: basic_text_widget{std::move(name), pos, alignment, font_size}, mousable{[=] { return std::string{localization[tooltip_key]}; }}
{
}

bool tooltippable_text_widget::holdable() const noexcept
{
	return false;
}