#pragma once
#include "global.hpp"

// Renderer for drawing blurred and desaturated images.
struct blur_renderer {
	blur_renderer(int texture_size);

	// Gets the render target corresponding to the input of the renderer.
	tr::gfx::render_target input();
	// Draws the blurred version of the image last renderered onto input.
	void draw(float saturation, float strength);

  private:
	tr::gfx::render_texture m_input_texture;
	tr::gfx::render_texture m_auxiliary_texture;
	tr::gfx::owning_shader_pipeline m_pipeline;
	tr::gfx::vertex_format m_vertex_format;
	tr::gfx::static_vertex_buffer<glm::i8vec2> m_vertex_buffer;
};