#include "engine.h"
static struct shader anim_vert_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/anim.vert",
#endif
.name = "anim_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 pos_from, pos_to, deriv_from, deriv_to;\n"
"uniform float pos_alpha;\n"
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"out vec2 posFrag;\n"
"void main()\n"
"{\n"
"//Spline calculation\n"
"vec2 a = deriv_from - (pos_to - pos_from),\n"
"b = -deriv_to + (pos_to - pos_from);\n"
"float inv_alpha = 1. - pos_alpha;\n"
"vec2 position = pos_from*inv_alpha + pos_to*pos_alpha\n"
"+ inv_alpha*pos_alpha*(a*inv_alpha + b*pos_alpha);\n"
"gl_Position = vec4((camera * transform * vec3(position, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(position, 1)).xy;\n"
"}\n"
};
shader_t anim_vert = &anim_vert_struct;

static struct shader blast_frag_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/blast.frag",
#endif
.name = "blast_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform float time;\n"
"uniform vec2 origin;\n"
"uniform sampler2DRect framebuffer;\n"
"in vec2 posFrag;\n"
"void main()\n"
"{\n"
"vec2 dir = posFrag - origin;\n"
"float amount = mod(sqrt(length(dir))*10 - time*4, 1);\n"
"vec2 displacement = amount*normalize(dir)*100;\n"
"color = texture(framebuffer, gl_FragCoord.xy + displacement);\n"
"}\n"
};
shader_t blast_frag = &blast_frag_struct;

static struct shader blit_frag_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/blit.frag",
#endif
.name = "blit_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform sampler2DRect framebuffer;\n"
"in vec2 posFrag;\n"
"void main()\n"
"{\n"
"color = texture(framebuffer, gl_FragCoord.xy);\n"
"}\n"
};
shader_t blit_frag = &blit_frag_struct;

static struct shader color_frag_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/color.frag",
#endif
.name = "color_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform vec3 main_color;\n"
"uniform sampler2D framebuffer;\n"
"void main()\n"
"{\n"
"color = vec4(main_color, 1);\n"
"}\n"
};
shader_t color_frag = &color_frag_struct;

static struct shader health_bar_frag_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/health bar.frag",
#endif
.name = "health_bar_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform float direction;\n"
"uniform float health, last_health, time_since_last_change;\n"
"in vec2 uv;\n"
"void main()\n"
"{\n"
"if (uv.x < health/100.)\n"
"color = vec4(1, 0, 0, 1);\n"
"else if (uv.x < last_health/100.) {\n"
"color = vec4(1, 1, 1, 1) * exp(-3.*time_since_last_change);\n"
"}\n"
"else\n"
"color = vec4(0, 0, 0, 0);\n"
"}\n"
};
shader_t health_bar_frag = &health_bar_frag_struct;

static struct shader particles_vert_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/particles.vert",
#endif
.name = "particles_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"uniform float time;\n"
"uniform vec2 init_vel;\n"
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"out vec2 posFrag;\n"
"int rand(int x) {\n"
"x ^= x << 13; x ^= x >> 17; x ^= x << 5;\n"
"return x;\n"
"}\n"
"void main()\n"
"{\n"
"vec2 pos = position*.005;\n"
"int seed = rand(gl_InstanceID) ^ int(1/fract(transform[0,2] + transform[1,2]));\n"
"int angle = rand(seed);\n"
"int vel = rand(angle);\n"
"float theta = float(angle) / pow(2., 20);\n"
"vec2 dir = vec2(cos(theta), sin(theta));\n"
"pos += time * (dir * (float(vel) / pow(2., 32)) + init_vel);\n"
"float gravity = time * time;\n"
"pos.y -= gravity;\n"
"gl_Position = vec4((camera * transform * vec3(pos, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(pos, 1)).xy;\n"
"}\n"
};
shader_t particles_vert = &particles_vert_struct;

static struct shader screenspace_vert_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/screenspace.vert",
#endif
.name = "screenspace_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"out vec2 posFrag;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position*2-1, 1, 1);\n"
"//posFrag is supposed to be in world space\n"
"posFrag = position*2-1;\n"
"}\n"
};
shader_t screenspace_vert = &screenspace_vert_struct;

static struct shader simple_vert_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/simple.vert",
#endif
.name = "simple_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"out vec2 posFrag;\n"
"out vec2 uv;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(\n"
"(camera * transform * vec3(position, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(position, 1)).xy;\n"
"uv = position;\n"
"}\n"
};
shader_t simple_vert = &simple_vert_struct;

static struct shader waves_frag_struct = {
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/waves.frag",
#endif
.name = "waves_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform float time;\n"
"uniform vec2 origin;\n"
"uniform vec3 main_color;\n"
"uniform vec3 lead_color;\n"
"uniform sampler2DRect framebuffer;\n"
"in vec2 posFrag;\n"
"void main()\n"
"{\n"
"float wave = distance(posFrag, origin)*5 - time*.6;\n"
"float pulse = 1;// - abs(cos(time*16)) * .2;\n"
"float val = pow(mod(wave, 1.), 2 * pulse) + .02;// - .3*rand(posFrag + time);\n"
"float val1 = 0.;//pow(1 - mod(wave, 1.), 16);\n"
"vec4 dest = texture(framebuffer, gl_FragCoord.xy);\n"
"float src_a = val + val1;\n"
"vec3 src = (main_color*val + lead_color*val1)/src_a;\n"
"float extra_a = max(dest.a - src_a, 0.);\n"
"src_a = max(src_a - extra_a, 0.);\n"
"color = dest * (1 - src_a) + vec4(src*src_a, src_a);\n"
"}\n"
};
shader_t waves_frag = &waves_frag_struct;

