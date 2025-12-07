#pragma once
#include "../global.hpp"

////////////////////////////////////////////////////////////// BLUR RENDERER //////////////////////////////////////////////////////////////

// Renderer for drawing blurred and desaturated images.
struct blur_renderer {
	// Crates a blur renderer.
	blur_renderer(int texture_size);

	// Gets the render target corresponding to the input of the renderer.
	tr::gfx::render_target input();

	// Draws the blurred version of the image last renderered onto input.
	void draw(float saturation, float strength);

  private:
	// Texture used as the input in the drawing process.
	tr::gfx::render_texture m_input_texture;
	// Helper texture used during the rendering process.
	tr::gfx::render_texture m_auxiliary_texture;
	// Shader pipeline used by the blue renderer.
	tr::gfx::owning_shader_pipeline m_pipeline;
	// Vertex format used by the blur renderer.
	tr::gfx::vertex_format m_vertex_format;
	// Vertex buffer used by the blur renderer.
	tr::gfx::static_vertex_buffer<glm::i8vec2> m_vertex_buffer;
};