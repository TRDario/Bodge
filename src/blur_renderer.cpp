#include "../include/blur_renderer.hpp"
#include "../include/graphics.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Minified version of src/shaders/pause_menu_background.vert.
constexpr const char* VERTEX_SHADER_SRC{"#version 450\n#define L(l) layout(location=l)\nL(0)in vec2 p;out gl_PerVertex{vec4 "
										"gl_Position;};L(0)out vec2 P;void main(){P=p;gl_Position=vec4(p,0,1);}"};
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
const std::uint32_t RENDERER_ID{tr::gfx::alloc_renderer_id()};
// The mesh used for drawing the pause menu background.
constexpr std::array<glm::i8vec2, 4> MESH{{{-1, 1}, {1, 1}, {1, -1}, {-1, -1}}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

blur_renderer::blur_renderer(int texture_size)
	: m_input_texture{glm::ivec2{texture_size}}
	, m_auxiliary_texture{glm::ivec2{texture_size}}
	, m_pipeline{tr::gfx::vertex_shader{VERTEX_SHADER_SRC}, tr::gfx::fragment_shader{FRAGMENT_SHADER_SRC}}
	, m_vertex_format{{tr::gfx::NOT_INSTANCED, {tr::gfx::vertex_attributef{tr::gfx::vertex_attributef::type::SI8, 2, false, 0}}}}
	, m_vertex_buffer{MESH}
{
	m_texture_unit.set_texture(m_input_texture);
	m_pipeline.fragment_shader().set_uniform(0, m_texture_unit);
	m_pipeline.fragment_shader().set_uniform(1, glm::vec2{m_input_texture.size()});
	if (tr::gfx::debug()) {
		m_input_texture.set_label("(Bodge) Blur Renderer Input Texture");
		m_auxiliary_texture.set_label("(Bodge) Blur Renderer Auxilliary Texture");
		m_pipeline.set_label("(Bodge) Blur Renderer Pipeline");
		m_pipeline.vertex_shader().set_label("(Bodge) Blur Renderer Vertex Shader");
		m_pipeline.fragment_shader().set_label("(Bodge) Blur Renderer Fragment Shader");
		m_vertex_format.set_label("(Bodge) Blur Renderer Vertex Format");
		m_vertex_buffer.set_label("(Bodge) Blur Renderer Vertex Buffer");
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

tr::gfx::render_target blur_renderer::input()
{
	m_input_texture.clear({});
	return m_input_texture;
}

void blur_renderer::draw(float saturation, float strength)
{
	strength = std::max(std::round(strength * engine::render_scale()), 2.0f);

	tr::gfx::set_renderer(RENDERER_ID);
	tr::gfx::set_shader_pipeline(m_pipeline);
	tr::gfx::set_vertex_format(m_vertex_format);
	tr::gfx::set_vertex_buffer(m_vertex_buffer, 0, 0);
	tr::gfx::set_blend_mode(tr::gfx::PREMUL_ALPHA_BLENDING);
	m_texture_unit.set_texture(m_input_texture);
	m_pipeline.fragment_shader().set_uniform(2, saturation);
	m_pipeline.fragment_shader().set_uniform(3, strength);
	m_pipeline.fragment_shader().set_uniform(4, 0);
	m_auxiliary_texture.clear({});
	tr::gfx::set_render_target(m_auxiliary_texture);
	tr::gfx::draw(tr::gfx::primitive::TRI_FAN, 0, 4);
	m_texture_unit.set_texture(m_auxiliary_texture);
	m_pipeline.fragment_shader().set_uniform(4, 1);
	tr::gfx::set_render_target(engine::screen());
	tr::gfx::draw(tr::gfx::primitive::TRI_FAN, 0, 4);
}