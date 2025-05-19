#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// UI OBJECT ////////////////////////////////////////////////////////////////

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

mousable::mousable(std::string&& tooltip) noexcept
	: tooltip{std::move(tooltip)}
{
}

////////////////////////////////////////////////////////////// SHORTCUTABLE ///////////////////////////////////////////////////////////////

shortcutable::shortcutable(tr::keycode chord_key, tr::keymods chord_mods) noexcept
	: chord_key{chord_key}, chord_mods{chord_mods}
{
}