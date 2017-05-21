#include "engine.h"
shader_t anim_vert = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/anim.vert",
#endif
.name = "anim_vert",
.type = GL_VERTEX_SHADER,
.source =
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"\n"
"out vec2 posFrag;\n"
"\n"
"vec2 skinned_pos();\n"
"\n"
"void main()\n"
"{\n"
"vec2 pos = skinned_pos();\n"
"gl_Position = vec4((camera * transform * vec3(pos, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(pos, 1)).xy;\n"
"}\n"
};
shader_t blast_frag = &(struct shader){
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
"\n"
"uniform sampler2DRect framebuffer;\n"
"\n"
"in vec2 posFrag;\n"
"\n"
"void main()\n"
"{\n"
"vec2 dir = posFrag - origin;\n"
"float amount = mod(sqrt(length(dir))*10 - time*4, 1);\n"
"vec2 displacement = amount*normalize(dir)*100;\n"
"\n"
"color = texture(framebuffer, gl_FragCoord.xy + displacement);\n"
"}\n"
};
shader_t blit_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/blit.frag",
#endif
.name = "blit_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"\n"
"uniform sampler2DRect framebuffer;\n"
"\n"
"in vec2 posFrag;\n"
"\n"
"void main()\n"
"{\n"
"color = texture(framebuffer, gl_FragCoord.xy);\n"
"}\n"
};
shader_t chevron_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/chevron.frag",
#endif
.name = "chevron_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform float time, direction;\n"
"uniform vec2 origin;\n"
"\n"
"uniform sampler2DRect framebuffer;\n"
"\n"
"in vec2 posFrag;\n"
"\n"
"void main()\n"
"{\n"
"vec2 dir = posFrag - origin;\n"
"float dist = dir.x;// + direction*abs(dir.y);\n"
"\n"
"float amount = dist*2;\n"
"\n"
"color = texture(framebuffer, gl_FragCoord.xy);\n"
"if (amount > .8 && amount < 1)\n"
"color += vec4(1,0,0,1);\n"
"}\n"
};
shader_t chevron_hit_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/chevron_hit.frag",
#endif
.name = "chevron_hit_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform float time, direction;\n"
"uniform vec2 origin;\n"
"\n"
"uniform sampler2DRect framebuffer;\n"
"\n"
"in vec2 posFrag;\n"
"\n"
"float snoise(vec2 v);\n"
"float srdnoise(in vec2 P, in float rot, out vec2 grad);\n"
"\n"
"void main()\n"
"{\n"
"float move_amt = time*.5;\n"
"const float xslope = 1, yslope = 1;\n"
"\n"
"vec2 dir = posFrag - origin;\n"
"vec2 moved_dir = dir - vec2(direction*move_amt, 0);\n"
"float dist = direction*dir.x + abs(dir.y);\n"
"float moved_dist = direction*moved_dir.x + abs(moved_dir.y);\n"
"vec2 grad = vec2(direction*xslope, yslope);\n"
"\n"
"float amount = mod(moved_dist*2, 1);\n"
"vec2 displacement = exp(amount*3)*normalize(grad)*10;\n"
"\n"
"vec2 noisegrad;\n"
"float noise = srdnoise(moved_dir*vec2(10,100), move_amt, noisegrad)*.2 + .3;\n"
"vec4 draw_color = vec4(0);\n"
"if (amount > .8)\n"
"draw_color = vec4((dist+1)*noise,0,0,1);\n"
"else if (amount < .1)\n"
"draw_color = vec4(.1-amount, 0, 0, .1-amount)*4*(dist+1);\n"
"\n"
"color = texture(framebuffer, gl_FragCoord.xy - displacement);\n"
"color = color*(1-draw_color.a) + draw_color;\n"
"}\n"
};
shader_t color_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/color.frag",
#endif
.name = "color_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"in vec2 posFrag;\n"
"out vec4 color;\n"
"uniform vec3 main_color;\n"
"uniform sampler2D framebuffer;\n"
"uniform float time;\n"
"\n"
"float snoise(vec2 v);\n"
"\n"
"void main()\n"
"{\n"
"//vec2 uv = gl_FragCoord.xy/3.;\n"
"//float noise = snoise(uv) + snoise(uv/2.+time) + snoise(uv/4.);\n"
"//color = vec4(vec3(noise*.2+.8), 1);\n"
"color = vec4(main_color, 1);\n"
"}\n"
};
shader_t health_bar_frag = &(struct shader){
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
"\n"
"in vec2 uv;\n"
"\n"
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
shader_t lib_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/lib.frag",
#endif
.name = "lib_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"const float pi = 3.14159265358979323846;\n"
"#define F2 0.366025403\n"
"#define G2 0.211324865\n"
"#define K 0.0243902439 // 1/41\n"
"\n"
"vec3 mod289(vec3 x) {\n"
"return x - floor(x * (1.0 / 289.0)) * 289.0;\n"
"}\n"
"\n"
"vec2 mod289(vec2 x) {\n"
"return x - floor(x * (1.0 / 289.0)) * 289.0;\n"
"}\n"
"\n"
"vec3 permute(vec3 x) {\n"
"return mod289(((x*34.0)+1.0)*x);\n"
"}\n"
"\n"
"float permute(float x) {\n"
"return mod((34.0 * x + 1.0)*x, 289.0);\n"
"}\n"
"\n"
"vec2 grad2(vec2 p, float rot) {\n"
"float u = permute(permute(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift\n"
"u = fract(u) * 6.28318530718; // 2*pi\n"
"return vec2(cos(u), sin(u));\n"
"}\n"
"\n"
"float snoise(vec2 v)\n"
"{\n"
"const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0\n"
"0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)\n"
"-0.577350269189626,  // -1.0 + 2.0 * C.x\n"
"0.024390243902439); // 1.0 / 41.0\n"
"vec2 i  = floor(v + dot(v, C.yy) );\n"
"vec2 x0 = v -   i + dot(i, C.xx);\n"
"vec2 i1;\n"
"i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);\n"
"vec4 x12 = x0.xyxy + C.xxzz;\n"
"x12.xy -= i1;\n"
"i = mod289(i); // Avoid truncation effects in permutation\n"
"vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))\n"
"+ i.x + vec3(0.0, i1.x, 1.0 ));\n"
"vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);\n"
"m = m*m ;\n"
"m = m*m ;\n"
"vec3 x = 2.0 * fract(p * C.www) - 1.0;\n"
"vec3 h = abs(x) - 0.5;\n"
"vec3 ox = floor(x + 0.5);\n"
"vec3 a0 = x - ox;\n"
"m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );\n"
"vec3 g;\n"
"g.x  = a0.x  * x0.x  + h.x  * x0.y;\n"
"g.yz = a0.yz * x12.xz + h.yz * x12.yw;\n"
"return 130.0 * dot(m, g);\n"
"}\n"
"\n"
"float srdnoise(in vec2 P, in float rot, out vec2 grad) {\n"
"vec2 Ps = P + dot(P, vec2(F2));\n"
"vec2 Pi = floor(Ps);\n"
"vec2 P0 = Pi - dot(Pi, vec2(G2));\n"
"vec2 v0 = P - P0;\n"
"vec2 i1 = (v0.x > v0.y) ? vec2(1.0, 0.0) : vec2 (0.0, 1.0);\n"
"vec2 v1 = v0 - i1 + G2;\n"
"vec2 v2 = v0 - 1.0 + 2.0 * G2;\n"
"Pi = mod(Pi, 289.0);\n"
"vec3 t = max(0.5 - vec3(dot(v0,v0), dot(v1,v1), dot(v2,v2)), 0.0);\n"
"vec3 t2 = t*t;\n"
"vec3 t4 = t2*t2;\n"
"vec2 g0 = grad2(Pi, rot);\n"
"vec2 g1 = grad2(Pi + i1, rot);\n"
"vec2 g2 = grad2(Pi + 1.0, rot);\n"
"vec3 gv = vec3(dot(g0,v0), dot(g1,v1), dot(g2,v2)); // ramp: g dot v\n"
"vec3 n = t4 * gv;  // Circular kernel times linear ramp\n"
"vec3 temp = t2 * t * gv;\n"
"vec3 gradx = temp * vec3(v0.x, v1.x, v2.x);\n"
"vec3 grady = temp * vec3(v0.y, v1.y, v2.y);\n"
"grad.x = -8.0 * (gradx.x + gradx.y + gradx.z);\n"
"grad.y = -8.0 * (grady.x + grady.y + grady.z);\n"
"grad.x += dot(t4, vec3(g0.x, g1.x, g2.x));\n"
"grad.y += dot(t4, vec3(g0.y, g1.y, g2.y));\n"
"grad *= 40.0;\n"
"return 40.0 * (n.x + n.y + n.z);\n"
"}\n"
};
shader_t lib_vert = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/lib.vert",
#endif
.name = "lib_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"in int bone, parent;\n"
"in float weight;\n"
"uniform float alpha;\n"
"uniform vec2 bones[32];\n"
"uniform vec4 bones_from[32], bones_to[32];\n"
"\n"
"mat2 rotation(float t) {\n"
"return mat2(cos(t), -sin(t),\n"
"sin(t), cos(t));\n"
"}\n"
"\n"
"vec2 skinned_pos() {\n"
"vec4 eff_bone = bones_from[bone] * (1. - alpha) + bones_to[bone] * alpha;\n"
"vec4 eff_parent = bones_from[parent] * (1. - alpha) + bones_to[parent] * alpha;\n"
"\n"
"float rel_bone_rot = (eff_bone.z - eff_parent.z) * (1 - weight);\n"
"float rot = eff_bone.z - rel_bone_rot;\n"
"vec2 bone_rel_pos = position - bones[bone];\n"
"bone_rel_pos = rotation(rot) * bone_rel_pos;\n"
"return eff_bone.xy + bone_rel_pos;\n"
"}\n"
"\n"
"\n"
"vec2 spline(vec2 a, vec2 da, vec2 b, vec2 db, float alpha)\n"
"{\n"
"vec2 x = da - (b - a),\n"
"y = -db + (b - a);\n"
"float inv_alpha = 1. - alpha;\n"
"return a*inv_alpha + b*alpha + inv_alpha*alpha*(x*inv_alpha + y*alpha);\n"
"}\n"
"\n"
"vec2 spline(vec2 a, vec2 da, vec2 b, vec2 db, float alpha, out vec2 deriv)\n"
"{\n"
"vec2 x = da - (b - a),\n"
"y = -db + (b - a);\n"
"float inv_alpha = 1. - alpha,\n"
"d_alpha1 = 2. - 3.*alpha,\n"
"d_alpha2 = 1. - 2.*alpha;\n"
"\n"
"deriv = b - a + d_alpha2*x + d_alpha1*(y*alpha - x*alpha);\n"
"return a*inv_alpha + b*alpha + inv_alpha*alpha*(x*inv_alpha + y*alpha);\n"
"}\n"
};
shader_t particles_vert = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/particles.vert",
#endif
.name = "particles_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"\n"
"uniform float time;\n"
"uniform vec2 init_vel;\n"
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"\n"
"out vec2 posFrag;\n"
"\n"
"int rand(int x) {\n"
"x ^= x << 13; x ^= x >> 17; x ^= x << 5;\n"
"return x;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"vec2 pos = position*.005;\n"
"\n"
"int seed = rand(gl_InstanceID) ^ int(1/fract(transform[0,2] + transform[1,2]));\n"
"int angle = rand(seed);\n"
"int vel = rand(angle);\n"
"\n"
"float theta = float(angle) / pow(2., 20);\n"
"vec2 dir = vec2(cos(theta), sin(theta));\n"
"\n"
"pos += time * (dir * (float(vel) / pow(2., 32)) + init_vel);\n"
"\n"
"float gravity = time * time;\n"
"pos.y -= gravity;\n"
"\n"
"gl_Position = vec4((camera * transform * vec3(pos, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(pos, 1)).xy;\n"
"\n"
"}\n"
};
shader_t screenspace_vert = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/screenspace.vert",
#endif
.name = "screenspace_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"out vec2 posFrag;\n"
"\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position*2-1, 1, 1);\n"
"//posFrag is supposed to be in world space\n"
"posFrag = position*2-1;\n"
"}\n"
};
shader_t simple_vert = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/simple.vert",
#endif
.name = "simple_vert",
.type = GL_VERTEX_SHADER,
.source =
"in vec2 position;\n"
"\n"
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"\n"
"out vec2 posFrag;\n"
"out vec2 uv;\n"
"\n"
"void main()\n"
"{\n"
"gl_Position = vec4(\n"
"(camera * transform * vec3(position, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(position, 1)).xy;\n"
"uv = position;\n"
"}\n"
};
shader_t state_indicator_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/state_indicator.frag",
#endif
.name = "state_indicator_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"in vec2 uv;\n"
"\n"
"uniform int top, bottom, top_attack, bottom_attack;\n"
"\n"
"void main()\n"
"{\n"
"if (uv.y > .5)\n"
"{\n"
"if (uv.x > .5)\n"
"color = vec4(1,0,0,1)*float(top_attack)*.33;\n"
"else\n"
"color = vec4(1,1,1,1)*float(top)*.33;\n"
"}\n"
"else\n"
"{\n"
"if (uv.x > .5)\n"
"color = vec4(1,0,0,1)*float(bottom_attack)*.33;\n"
"else\n"
"color = vec4(1,1,1,1)*float(bottom)*.33;\n"
"}\n"
"}\n"
};
shader_t waves_frag = &(struct shader){
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
"\n"
"uniform sampler2DRect framebuffer;\n"
"\n"
"in vec2 posFrag;\n"
"\n"
"void main()\n"
"{\n"
"float wave = distance(posFrag, origin)*5 - time*.6;\n"
"float pulse = 1;// - abs(cos(time*16)) * .2;\n"
"float val = pow(mod(wave, 1.), 2 * pulse) + .02;// - .3*rand(posFrag + time);\n"
"\n"
"float val1 = 0.;//pow(1 - mod(wave, 1.), 16);\n"
"\n"
"vec4 dest = texture(framebuffer, gl_FragCoord.xy);\n"
"float src_a = val + val1;\n"
"vec3 src = (main_color*val + lead_color*val1)/src_a;\n"
"float extra_a = max(dest.a - src_a, 0.);\n"
"src_a = max(src_a - extra_a, 0.);\n"
"color = dest * (1 - src_a) + vec4(src*src_a, src_a);\n"
"}\n"
};
