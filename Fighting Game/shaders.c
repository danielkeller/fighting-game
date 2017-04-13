#include "shaders.h"
#include "gl_core_3_3.h"
static struct shader anim_vert_struct = {
.shader = 0,
.name = "anim_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 pos_from, pos_to, deriv_from, deriv_to;\n"
"uniform float pos_alpha;\n"
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"out vec2 posFrag;\n"
"out vec2 framebuffer_coord;\n"
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
"framebuffer_coord = gl_Position.xy / 2. + 0.5;\n"
"}\n"
};
shader_t anim_vert = &anim_vert_struct;

static struct shader color_frag_struct = {
.shader = 0,
.name = "color_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform vec3 main_color;\n"
"uniform sampler2D framebuffer;\n"
"in vec2 posFrag;\n"
"in vec2 framebuffer_coord;\n"
"void main()\n"
"{\n"
"color = vec4(main_color, 1);\n"
"}\n"
};
shader_t color_frag = &color_frag_struct;

static struct shader health_bar_vert_struct = {
.shader = 0,
.name = "health_bar_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"uniform float direction;\n"
"uniform float health;\n"
"out vec2 posFrag;\n"
"out vec2 framebuffer_coord;\n"
"void main()\n"
"{\n"
"mat3 transform = mat3(\n"
".8*direction*(health / 100.),\n"
"0, 0,\n"
"0, .1, 0,\n"
".1*direction, .8, 1);\n"
"gl_Position = vec4((transform * vec3(position, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(position, 1)).xy;\n"
"framebuffer_coord = gl_Position.xy / 2. + 0.5;\n"
"}\n"
};
shader_t health_bar_vert = &health_bar_vert_struct;

static struct shader simple_vert_struct = {
.shader = 0,
.name = "simple_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"out vec2 posFrag;\n"
"out vec2 framebuffer_coord;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(\n"
"(camera * transform * vec3(position, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(position, 1)).xy;\n"
"framebuffer_coord = gl_Position.xy / 2. + 0.5;\n"
"}\n"
};
shader_t simple_vert = &simple_vert_struct;

static struct shader waves_frag_struct = {
.shader = 0,
.name = "waves_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform float time;\n"
"uniform vec2 origin;\n"
"uniform vec3 main_color;\n"
"uniform vec3 lead_color;\n"
"uniform sampler2D framebuffer;\n"
"in vec2 posFrag;\n"
"in vec2 framebuffer_coord;\n"
"void main()\n"
"{\n"
"float wave = distance(posFrag, origin)*5 - time*.6;\n"
"float pulse = 1;// - abs(cos(time*16)) * .2;\n"
"float val = pow(mod(wave, 1.), 2 * pulse) + .02;// - .3*rand(posFrag + time);\n"
"float val1 = 0.;//pow(1 - mod(wave, 1.), 16);\n"
"vec4 dest = texture(framebuffer, framebuffer_coord);\n"
"float src_a = val + val1;\n"
"vec3 src = (main_color*val + lead_color*val1)/src_a;\n"
"float extra_a = max(dest.a - src_a, 0.);\n"
"src_a = max(src_a - extra_a, 0.);\n"
"color = dest * (1 - src_a) + vec4(src*src_a, src_a);\n"
"}\n"
};
shader_t waves_frag = &waves_frag_struct;

