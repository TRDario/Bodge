#include "../include/blur_renderer.hpp"
#include "../include/engine.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Minified version of src/shaders/pause_menu_background.vert.
constexpr const char* VERTEX_SHADER_SRC{"#version 450\n#define L(l) layout(location=l)\nL(0)in vec2 p;L(1)in vec4 c;L(0)out "
										"vec2 P;void main(){P=p;gl_Position=vec4(p,0,1);}"};
// Minified version of src/shaders/pause_menu_background.frag.
constexpr const char* FRAGMENT_SHADER_SRC{
	"#version 450\n#define L(l) layout(location=l)\nL(0)in vec2 p;L(0)out vec4 C;L(0)uniform sampler2D t;L(1)uniform vec2 S;L(2)uniform "
	"float s;L(3)uniform float r;L(4)uniform int a;vec3 H(vec3 c){vec4 "
	"K=vec4(0,-1/3.0,2/"
	"3.0,-1),p=mix(vec4(c.bg,K.wz),vec4(c.gb,K.xy),step(c.b,c.g)),q=mix(vec4(p.xyw,c.r),vec4(c.r,p.yzx),step(p.x,c.r));float "
	"d=q.x-min(q.w, q.y),e=1.0e-10;return vec3(abs(q.z+(q.w-q.y)/(6*d+e)),d/(q.x+e),q.x);}vec3 G(vec3 c){vec4 K=vec4(1,2/3.0,1/3.0,3);vec3 "
	"p=abs(fract(c.xxx+K.xyz)*6-K.www);return c.z*mix(K.xxx,clamp(p-K.xxx,0,1),c.y);}void main(){float x,y,R=r*r,d,w,W;vec2 "
	"p=0.5*(vec2(1)+p);vec4 "
	"k=vec4(0);W=0.5135/pow(r,0.96);if(a==0){for(d=1/S.x,x=-r,p.x+=x*d;x<=r;x++,p.x+=d){w=W*exp((-x*x)/"
	"(2*R));k+=texture(t,p)*w;}C=k;}else{for(d=1/S.y,y=-r,p.y+=y*d;y<=r;y++,p.y+=d){w=W*exp((-y*y)/"
	"(2*R));k+=texture(t,p)*w;}vec3 g=H(k.rgb);C=vec4(G(vec3(g.x,g.y*s,g.z)),1);}}"};
// The renderer ID of the pause menu background renderer.
const u32 RENDERER_ID{tr::alloc_renderer_id()};
// The mesh used for drawing the pause menu background.
constexpr array<clrvtx, 4> MESH{{{{-1, 1}}, {{1, 1}}, {{1, -1}}, {{-1, -1}}}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

blur_renderer::blur_renderer(int texture_size)
	: _input_tex{glm::ivec2{texture_size}}
	, _aux_tex{glm::ivec2{texture_size}}
	, _pipeline{tr::vertex_shader{VERTEX_SHADER_SRC}, tr::fragment_shader{FRAGMENT_SHADER_SRC}}
	, _vbuffer{MESH}
{
	_tex_unit.set_texture(_input_tex);
	_pipeline.fragment_shader().set_uniform(0, _tex_unit);
	_pipeline.fragment_shader().set_uniform(1, static_cast<vec2>(_input_tex.size()));

	if (cli_settings.debug_mode) {
		_input_tex.set_label("(Bodge) Blur Renderer Input Texture");
		_aux_tex.set_label("(Bodge) Blur Renderer Auxilliary Texture");
		_pipeline.set_label("(Bodge) Blur Renderer Pipeline");
		_pipeline.vertex_shader().set_label("(Bodge) Blur Renderer Vertex Shader");
		_pipeline.fragment_shader().set_label("(Bodge) Blur Renderer Fragment Shader");
		_vbuffer.set_label("(Bodge) Blur Renderer Vertex Buffer");
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

tr::render_target blur_renderer::input() noexcept
{
	_input_tex.clear({});
	return _input_tex;
}

void blur_renderer::draw(float saturation, float strength) noexcept
{
	strength = max(std::round(strength * engine::render_scale()), 2.0f);

	tr::gfx_context::set_renderer(RENDERER_ID);
	tr::gfx_context::set_shader_pipeline(_pipeline);
	tr::gfx_context::set_vertex_buffer(_vbuffer, 0);
	tr::gfx_context::set_blend_mode(tr::PREMUL_ALPHA_BLENDING);
	_tex_unit.set_texture(_input_tex);
	_pipeline.fragment_shader().set_uniform(2, saturation);
	_pipeline.fragment_shader().set_uniform(3, strength);
	_pipeline.fragment_shader().set_uniform(4, 0);
	_aux_tex.clear({});
	tr::gfx_context::set_render_target(_aux_tex);
	tr::gfx_context::draw(tr::primitive::TRI_FAN, 0, 4);
	_tex_unit.set_texture(_aux_tex);
	_pipeline.fragment_shader().set_uniform(4, 1);
	tr::gfx_context::set_render_target(engine::screen());
	tr::gfx_context::draw(tr::primitive::TRI_FAN, 0, 4);
}