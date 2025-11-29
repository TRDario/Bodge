#pragma once
#include "../fonts.hpp"
#include "../system.hpp"
#include "interpolator.hpp"

////////////////////////////////////////////////////////////////// COMMON /////////////////////////////////////////////////////////////////

using text_callback = std::function<std::string()>;
using status_callback = std::function<bool()>;
using action_callback = std::function<void()>;
template <class T> using validation_callback = std::function<T(std::common_type_t<T, int>)>;

inline const text_callback NO_TOOLTIP{};
// Common callback for text widgets: getting a localization string using the widget name as the key.
struct tag_loc {
	tag tag;

	std::string operator()() const;
};
// Common callback for text widgets: getting a tooltip localization string.
struct tag_tooltip_loc {
	tag tag;

	std::string operator()() const;
};
// Common callback for text widgets: copying a string directly.
struct copy_string {
	std::string str;

	std::string operator()() const;
};

// Common validation callback: clamp between two values.
template <class T, T Min, T Max> struct clamp_validation {
	T operator()(std::common_type_t<T, int> v) const
	{
		return T(std::clamp<std::common_type_t<T, int>>(v, Min, Max));
	}
};

constexpr ticks DONT_UNHIDE{std::numeric_limits<ticks>::max()};

////////////////////////////////////////////////////////////////// WIDGET /////////////////////////////////////////////////////////////////

class widget {
  public:
	widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, bool writable);
	virtual ~widget() = default;

	tr::align alignment;
	tweener<glm::vec2> pos;
	text_callback tooltip_cb;

	virtual glm::vec2 size() const = 0;
	glm::vec2 tl() const;

	float opacity() const;
	void hide();
	void hide(ticks time);
	void unhide();
	void unhide(ticks time);

	bool hidden() const;
	virtual bool interactible() const;
	bool writable() const;

	virtual void on_action() {}
	virtual void on_hover() {}
	virtual void on_unhover() {}
	virtual void on_held() {}
	virtual void on_unheld() {}
	virtual void on_selected() {};
	virtual void on_unselected() {};
	virtual void on_write(std::string_view){};
	virtual void on_enter() {};
	virtual void on_erase() {};
	virtual void on_clear() {};
	virtual void on_copy() {};
	virtual void on_paste() {};

	virtual void update();
	virtual void release_graphical_resources() {}
	virtual void add_to_renderer() = 0;

  private:
	tweener<float> m_opacity;
	const bool m_writable;
};

/////////////////////////////////////////////////////////////// TEXT WIDGET ///////////////////////////////////////////////////////////////

class text_widget : public widget {
  public:
	text_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, bool writable,
				text_callback text_cb, font font, text_style style, float font_size, int max_width);

	text_callback text_cb;

	glm::vec2 size() const override;
	void release_graphical_resources() override;

  protected:
	font m_font;
	text_style m_style;
	float m_font_size;
	int m_max_width;
	mutable std::string m_last_text;
	mutable std::variant<std::monostate, tr::bitmap, tr::gfx::texture> m_cache;
	mutable glm::vec2 m_last_size;

	void update_cache() const;
	void add_to_renderer_raw(tr::rgba8 tint);
};