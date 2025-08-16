#pragma once
#include "../audio.hpp"
#include "../graphics.hpp"
#include "ui_manager.hpp"
#include "widget.hpp"

/////////////////////////////////////////////////////////// NUMERIC INPUT WIDGET //////////////////////////////////////////////////////////

template <class T, tr::template_string_literal Fmt, tr::template_string_literal BufferFmt>
void default_numeric_input_formatter<T, Fmt, BufferFmt>::from_string(T& out, std::string_view str)
{
	std::from_chars(str.data(), str.data() + str.size(), out);
}

template <class T, tr::template_string_literal Fmt, tr::template_string_literal BufferFmt>
std::string default_numeric_input_formatter<T, Fmt, BufferFmt>::to_string(T v)
{
	return std::format(Fmt, v);
}

template <class T, tr::template_string_literal Fmt, tr::template_string_literal BufferFmt>
std::string default_numeric_input_formatter<T, Fmt, BufferFmt>::to_string(std::string_view str)
{
	return std::format(BufferFmt, str);
}

template <class T, std::size_t S, class Formatter>
basic_numeric_input_widget<T, S, Formatter>::basic_numeric_input_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, float font_size,
												 ui_manager& ui, T& ref, status_callback status_cb,
												 validation_callback<T> validation_cb)
	: text_widget_base{
		  pos,
		  alignment,
		  unhide_time,
		  NO_TOOLTIP,
		  true,
		  [this] {
			  if (m_selected) {
				  if (m_buffer.empty()) {
					  return std::string{"..."};
				  }
				  else {
					  return Formatter::to_string(m_buffer);
				  }
			  }
			  else {
				  return Formatter::to_string(m_ref);
			  }
		  },
		  font::LANGUAGE,
		  tr::system::ttf_style::NORMAL,
		  font_size,
		  tr::system::UNLIMITED_WIDTH,
	  }
	  , m_ui{ui}
	  , m_ref{ref}
	  , m_scb{std::move(status_cb)}
	  , m_vcb{std::move(validation_cb)}
	  , m_interp{m_scb() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8}
	  , m_hovered{false}
	  , m_held{false}
	  , m_selected{false}
{
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::add_to_renderer()
{
	text_widget_base::add_to_renderer_raw(m_interp);
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::update()
{
	text_widget_base::update();
	m_interp.update();

	if (interactible()) {
		if (interactible() && !(m_held || m_hovered || m_selected) && m_interp.done() && m_interp != "A0A0A0A0"_rgba8) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
		else if (m_interp.done() && (m_hovered || m_selected) && !m_held) {
			m_interp.change(interp::CYCLE, tr::color_cast<tr::rgba8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 0.2f, 1.0f}),
							4_s);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if (m_interp.done() && m_interp != "505050A0"_rgba8) {
			m_interp.change(interp::LERP, "505050A0"_rgba8, 0.1_s);
		}
	}
}

template <class T, std::size_t S, class Formatter> bool basic_numeric_input_widget<T, S, Formatter>::interactible() const
{
	return m_scb();
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_action()
{
	m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
			engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_held()
{
	if (interactible()) {
		m_held = true;
		m_interp = "202020"_rgba8;
	}
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
		}
		else {
			engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}

		m_buffer.clear();
	}
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}

		T temp{m_ref};
		Formatter::from_string(temp, m_buffer);
		m_ref = m_vcb(temp);
	}
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_write(std::string_view input)
{
	if (input.size() == 1 && (std::isdigit(input.front()) || input.front() == '.') && m_buffer.size() < S) {
		m_buffer.append(input);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_enter()
{
	m_ui.set_selection(nullptr);
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_erase()
{
	if (!m_buffer.empty()) {
		m_buffer.pop_back();
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <class T, std::size_t S, class Formatter> void basic_numeric_input_widget<T, S, Formatter>::on_clear()
{
	m_buffer.clear();
	engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
}

//////////////////////////////////////////////////////////// LINE INPUT WIDGET ////////////////////////////////////////////////////////////

template <std::size_t S>
line_input_widget<S>::line_input_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, tr::system::ttf_style style,
										float font_size, status_callback status_cb, action_callback enter_cb, std::string_view initial_text)
	: text_widget_base{pos,
					   alignment,
					   unhide_time,
					   NO_TOOLTIP,
					   true,
					   [this] { return buffer.empty() ? std::string{engine::loc["empty"]} : std::string{buffer}; },
					   font::LANGUAGE,
					   style,
					   font_size,
					   tr::system::UNLIMITED_WIDTH}
	, buffer{initial_text}
	, m_scb{std::move(status_cb)}
	, m_enter_cb{std::move(enter_cb)}
	, m_interp{m_scb() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
{
}

template <std::size_t S> void line_input_widget<S>::add_to_renderer()
{
	if (buffer.empty()) {
		tr::rgba8 color{m_interp};
		color.r /= 2;
		color.g /= 2;
		color.b /= 2;
		text_widget_base::add_to_renderer_raw(color);
	}
	else {
		text_widget_base::add_to_renderer_raw(m_interp);
	}
}

template <std::size_t S> void line_input_widget<S>::update()
{
	text_widget_base::update();
	m_interp.update();

	if (interactible()) {
		if (interactible() && !(m_held || m_hovered || m_selected) && m_interp.done() && m_interp != "A0A0A0A0"_rgba8) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
		else if (m_interp.done() && (m_hovered || m_selected) && !m_held) {
			m_interp.change(interp::CYCLE, tr::color_cast<tr::rgba8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 0.2f, 1.0f}),
							4_s);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if (m_interp.done() && m_interp != "505050A0"_rgba8) {
			m_interp.change(interp::LERP, "505050A0"_rgba8, 0.1_s);
		}
	}
}

template <std::size_t S> bool line_input_widget<S>::interactible() const
{
	return m_scb();
}

template <std::size_t S> void line_input_widget<S>::on_action()
{
	m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
}

template <std::size_t S> void line_input_widget<S>::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
			engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

template <std::size_t S> void line_input_widget<S>::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

template <std::size_t S> void line_input_widget<S>::on_held()
{
	if (interactible()) {
		m_held = true;
		m_interp = "202020"_rgba8;
	}
}

template <std::size_t S> void line_input_widget<S>::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

template <std::size_t S> void line_input_widget<S>::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
		}
		else {
			engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

template <std::size_t S> void line_input_widget<S>::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

template <std::size_t S> void line_input_widget<S>::on_write(std::string_view input)
{
	if (tr::utf8::length(buffer) + tr::utf8::length(input) <= S) {
		buffer.append(input);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <std::size_t S> void line_input_widget<S>::on_enter()
{
	m_enter_cb();
}

template <std::size_t S> void line_input_widget<S>::on_erase()
{
	if (!buffer.empty()) {
		tr::utf8::pop_back(buffer);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <std::size_t S> void line_input_widget<S>::on_clear()
{
	buffer.clear();
	engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
}

template <std::size_t S> void line_input_widget<S>::on_copy()
{
	tr::system::set_clipboard_text(std::string{buffer});
}

template <std::size_t S> void line_input_widget<S>::on_paste()
{
	const std::size_t buffer_length{tr::utf8::length(buffer)};
	if (!tr::system::clipboard_empty() && buffer_length < S) {
		std::string pasted{tr::system::clipboard_text()};
		std::erase(pasted, '\n');
		buffer += (buffer_length + tr::utf8::length(pasted) > S)
					  ? std::string_view{pasted.begin(), tr::utf8::next(pasted.begin(), S - buffer_length)}
					  : pasted;
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

////////////////////////////////////////////////////////////// REPLAY WIDGET //////////////////////////////////////////////////////////////

replay_widget::replay_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, auto base_scb, auto base_acb,
							 std::optional<std::map<std::string, replay_header>::iterator> it)
	: text_button_widget{pos,
						 alignment,
						 unhide_time,
						 [this] {
							 if (!this->it.has_value()) {
								 return std::string{};
							 }
							 else {
								 const replay_header& header{(*this->it)->second};
								 std::string str{header.description};
								 if ((header.flags.exited_prematurely || header.flags.modified_game_speed) && !str.empty()) {
									 str.push_back('\n');
								 }
								 if (header.flags.exited_prematurely) {
									 if (!str.empty()) {
										 str.push_back('\n');
									 }
									 str.append(engine::loc["exited_prematurely"]);
								 }
								 if (header.flags.modified_game_speed) {
									 if (!str.empty()) {
										 str.push_back('\n');
									 }
									 str.append(engine::loc["modified_game_speed"]);
								 }
								 return str;
							 }
						 },
						 [this] {
							 if (!this->it.has_value()) {
								 return std::string{"----------------------------------"};
							 }

							 replay_header& rpy{(*this->it)->second};
							 const ticks result{rpy.result};
							 const ticks result_m{result / 60_s};
							 const ticks result_s{(result % 60_s) / 1_s};
							 const ticks result_ms{(result % 1_s) * 100 / 1_s};
							 const std::chrono::system_clock::time_point utc_tp{std::chrono::seconds{rpy.timestamp}};
							 const auto tp{std::chrono::current_zone()->std::chrono::time_zone::to_local(utc_tp)};
							 const std::chrono::hh_mm_ss hhmmss{tp - std::chrono::floor<std::chrono::days>(tp)};
							 const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(tp)};
							 const int year{ymd.year()};
							 const unsigned int month{ymd.month()};
							 const unsigned int day{ymd.day()};
							 const auto hour{hhmmss.hours().count()};
							 const auto minute{hhmmss.minutes().count()};
							 return std::format("{} ({}: {})\n{} | {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", rpy.name,
												engine::loc["by"], rpy.player, ::name(rpy.gamemode), result_m, result_s, result_ms, year,
												month, day, hour, minute);
						 },
						 font::LANGUAGE,
						 40,
						 [=, this] { return base_scb() && this->it.has_value(); },
						 [=, this] {
							 if (this->it.has_value()) {
								 base_acb(*this->it);
							 }
						 },
						 sound::CONFIRM}
	, it{it}
{
}

////////////////////////////////////////////////////////// MULTILINE TEXT INPUT ///////////////////////////////////////////////////////////

template <std::size_t S>
multiline_input_widget<S>::multiline_input_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, float width,
												  std::uint8_t max_lines, float font_size, status_callback status_cb)
	: text_widget_base{pos,
					   alignment,
					   unhide_time,
					   NO_TOOLTIP,
					   true,
					   [this] { return buffer.empty() ? std::string{engine::loc["empty"]} : std::string{buffer}; },
					   font::LANGUAGE,
					   tr::system::ttf_style::NORMAL,
					   font_size,
					   static_cast<int>(width)}
	, m_scb{std::move(status_cb)}
	, m_size{width, engine::line_skip(font::LANGUAGE, font_size) * max_lines + 4}
	, m_max_lines{max_lines}
	, m_interp{m_scb() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8}
	, m_hovered{false}
	, m_held{false}
	, m_selected{false}
{
}

template <std::size_t S> glm::vec2 multiline_input_widget<S>::size() const
{
	return m_size;
}

template <std::size_t S> void multiline_input_widget<S>::add_to_renderer()
{
	tr::rgba8 color{m_interp};
	if (buffer.empty()) {
		color.r /= 2;
		color.g /= 2;
		color.b /= 2;
		text_widget_base::add_to_renderer_raw(color);
	}
	else {
		text_widget_base::add_to_renderer_raw(m_interp);
	}
	color.a *= opacity();

	const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::UI, 4)};
	tr::fill_rect_outline_vtx(outline.positions, {tl() + 1.0f, size() - 2.0f}, 2.0f);
	std::ranges::fill(outline.colors, color);
	const tr::gfx::simple_color_mesh_ref fill{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(fill.positions, {tl() + 2.0f, size() - 4.0f});
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, static_cast<std::uint8_t>(160 * opacity())});
}

template <std::size_t S> void multiline_input_widget<S>::update()
{
	text_widget_base::update();
	m_interp.update();

	if (interactible()) {
		if (interactible() && !(m_held || m_hovered || m_selected) && m_interp.done() && m_interp != "A0A0A0A0"_rgba8) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
		else if (m_interp.done() && (m_hovered || m_selected) && !m_held) {
			m_interp.change(interp::CYCLE, tr::color_cast<tr::rgba8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 0.2f, 1.0f}),
							4_s);
		}
	}
	else {
		m_hovered = false;
		m_held = false;
		m_selected = false;
		if (m_interp.done() && m_interp != "505050A0"_rgba8) {
			m_interp.change(interp::LERP, "505050A0"_rgba8, 0.1_s);
		}
	}
}

template <std::size_t S> bool multiline_input_widget<S>::interactible() const
{
	return m_scb();
}

template <std::size_t S> void multiline_input_widget<S>::on_action()
{
	m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
}

template <std::size_t S> void multiline_input_widget<S>::on_hover()
{
	if (interactible()) {
		m_hovered = true;
		if (!m_selected) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
			engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_unhover()
{
	if (interactible()) {
		m_hovered = false;
		if (!m_selected) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_held()
{
	if (interactible()) {
		m_held = true;
		m_interp = "202020"_rgba8;
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_unheld()
{
	if (interactible()) {
		m_held = false;
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_selected()
{
	if (interactible()) {
		m_selected = true;
		if (!m_hovered) {
			m_interp.change(interp::LERP, "FFFFFF"_rgba8, 0.1_s);
		}
		else {
			engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
		}
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_unselected()
{
	if (interactible()) {
		m_selected = false;
		if (!m_hovered) {
			m_interp.change(interp::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		}
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_write(std::string_view input)
{
	if (tr::utf8::length(buffer) + tr::utf8::length(input) <= S) {
		buffer.append(input);
		if (engine::count_lines(buffer, font::LANGUAGE, tr::system::ttf_style::NORMAL, m_font_size, m_font_size / 12, m_size.x) >
			m_max_lines) {
			buffer.resize(buffer.size() - input.size());
		}
		else {
			engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
		}
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_enter()
{
	if (tr::utf8::length(buffer) < S &&
		engine::count_lines(buffer, font::LANGUAGE, tr::system::ttf_style::NORMAL, m_font_size, m_font_size / 12, m_size.x) < m_max_lines) {
		buffer.append('\n');
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_erase()
{
	if (!buffer.empty()) {
		tr::utf8::pop_back(buffer);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_clear()
{
	buffer.clear();
	engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
}

template <std::size_t S> void multiline_input_widget<S>::on_copy()
{
	tr::system::set_clipboard_text(std::string{buffer}.c_str());
}

template <std::size_t S> void multiline_input_widget<S>::on_paste()
{
	try {
		if (!tr::system::clipboard_empty()) {
			std::string pasted{tr::system::clipboard_text()};
			tr::static_string<S * 4> copy{buffer};
			const std::size_t buffer_length{tr::utf8::length(buffer)};
			copy += (buffer_length + tr::utf8::length(pasted) > S)
						? std::string_view{pasted.begin(), tr::utf8::next(pasted.begin(), S - buffer_length)}
						: pasted;
			// Replace this with a smarter solution eventually, maybe.
			if (engine::count_lines(copy, font::LANGUAGE, tr::system::ttf_style::NORMAL, m_font_size, m_font_size / 12, m_size.x) <=
				m_max_lines) {
				buffer = copy;
			}
			engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
		}
	}
	catch (...) {
	}
}