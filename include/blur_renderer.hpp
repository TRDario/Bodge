#pragma once
#include "global.hpp"

// Renderer for drawing blurred and desaturated images.
struct blur_renderer {
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates the blur renderer.
	blur_renderer(int texture_size);

	/////////////////////////////////////////////////////////////// METHODS ///////////////////////////////////////////////////////////////

	// Gets the render target corresponding to the input of the renderer.
	tr::render_target input() noexcept;
	// Draws the blurred version of the image last renderered onto input.
	void draw(float saturation, float strength) noexcept;

  private:
	// Texture used as input for blur rendering.
	tr::render_texture _input_tex;
	// Texture used as an auxiliary for blur rendering.
	tr::render_texture _aux_tex;
	// Texture unit used for blur rendering.
	tr::texture_unit _tex_unit;
	// Shader pipeline used for blur rendering.
	tr::owning_shader_pipeline _pipeline;
	// Vertex buffer used for blur rendering.
	tr::static_vertex_buffer<tr::clrvtx2> _vbuffer;
};