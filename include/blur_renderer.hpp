#pragma once
#include "global.hpp"

// Renderer for drawing blurred and desaturated images.
struct blur_renderer {
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates the blur renderer.
	blur_renderer(int texture_size);

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Gets the render target corresponding to the input of the renderer.
	tr::render_target input();
	// Draws the blurred version of the image last renderered onto input.
	void draw(float saturation, float strength);

  private:
	// Texture used as input for blur rendering.
	tr::render_texture m_input_texture;
	// Texture used as an auxiliary for blur rendering.
	tr::render_texture m_auxiliary_texture;
	// Texture unit used for blur rendering.
	tr::texture_unit m_texture_unit;
	// Shader pipeline used for blur rendering.
	tr::owning_shader_pipeline m_pipeline;
	// The vertex foramt used for blur rendering.
	tr::vertex_format m_vertex_format;
	// Vertex buffer used for blur rendering.
	tr::static_vertex_buffer<glm::i8vec2> m_vertex_buffer;
};