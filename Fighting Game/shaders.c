#include "engine.h"
shader_t anim_vert = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/anim.vert",
#endif
.name = "anim_vert",
.type = GL_VERTEX_SHADER,
.source =
"vec2 skinned_pos();\n"
"void output_local_world_space(vec2 pos);\n"
"\n"
"void main()\n"
"{\n"
"output_local_world_space(skinned_pos());\n"
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
shader_t blur_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/blur.frag",
#endif
.name = "blur_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform sampler2DRect framebuffer;\n"
"uniform sampler2DRect noise;\n"
"in vec2 posFrag;\n"
"\n"
"void main()\n"
"{\n"
"vec2 noise_coord = gl_FragCoord.xy / textureSize(framebuffer) * textureSize(noise);\n"
"vec4 noise_val = texture(noise, noise_coord);\n"
"color = texture(framebuffer, gl_FragCoord.xy + (noise_val.xy - .5) * 20);\n"
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
"vec4 mod289(vec4 x) {\n"
"return x - floor(x * (1.0 / 289.0)) * 289.0;\n"
"}\n"
"\n"
"vec4 permute(vec4 x) {\n"
"return mod289(((x*34.0)+1.0)*x);\n"
"}\n"
"\n"
"vec4 taylorInvSqrt(vec4 r)\n"
"{\n"
"return 1.79284291400159 - 0.85373472095314 * r;\n"
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
"\n"
"\n"
"float snoise(vec3 v)\n"
"{\n"
"const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;\n"
"const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);\n"
"\n"
"// First corner\n"
"vec3 i  = floor(v + dot(v, C.yyy) );\n"
"vec3 x0 =   v - i + dot(i, C.xxx) ;\n"
"\n"
"// Other corners\n"
"vec3 g = step(x0.yzx, x0.xyz);\n"
"vec3 l = 1.0 - g;\n"
"vec3 i1 = min( g.xyz, l.zxy );\n"
"vec3 i2 = max( g.xyz, l.zxy );\n"
"vec3 x1 = x0 - i1 + C.xxx;\n"
"vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y\n"
"vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y\n"
"\n"
"// Permutations\n"
"i = mod289(i);\n"
"vec4 p = permute( permute( permute(\n"
"i.z + vec4(0.0, i1.z, i2.z, 1.0 ))\n"
"+ i.y + vec4(0.0, i1.y, i2.y, 1.0 ))\n"
"+ i.x + vec4(0.0, i1.x, i2.x, 1.0 ));\n"
"\n"
"float n_ = 0.142857142857; // 1.0/7.0\n"
"vec3  ns = n_ * D.wyz - D.xzx;\n"
"\n"
"vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)\n"
"\n"
"vec4 x_ = floor(j * ns.z);\n"
"vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)\n"
"\n"
"vec4 x = x_ *ns.x + ns.yyyy;\n"
"vec4 y = y_ *ns.x + ns.yyyy;\n"
"vec4 h = 1.0 - abs(x) - abs(y);\n"
"\n"
"vec4 b0 = vec4( x.xy, y.xy );\n"
"vec4 b1 = vec4( x.zw, y.zw );\n"
"\n"
"vec4 s0 = floor(b0)*2.0 + 1.0;\n"
"vec4 s1 = floor(b1)*2.0 + 1.0;\n"
"vec4 sh = -step(h, vec4(0.0));\n"
"\n"
"vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;\n"
"vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;\n"
"\n"
"vec3 p0 = vec3(a0.xy,h.x);\n"
"vec3 p1 = vec3(a0.zw,h.y);\n"
"vec3 p2 = vec3(a1.xy,h.z);\n"
"vec3 p3 = vec3(a1.zw,h.w);\n"
"\n"
"vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));\n"
"p0 *= norm.x;\n"
"p1 *= norm.y;\n"
"p2 *= norm.z;\n"
"p3 *= norm.w;\n"
"\n"
"vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);\n"
"m = m * m;\n"
"return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),\n"
"dot(p2,x2), dot(p3,x3) ) );\n"
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
"const float pi = 3.1415926535897;\n"
"\n"
"mat2 rotation(float t) {\n"
"return mat2(cos(t), -sin(t),\n"
"sin(t), cos(t));\n"
"}\n"
"\n"
"in vec2 position;\n"
"in int bone, parent;\n"
"in float weight;\n"
"uniform float alpha;\n"
"uniform vec4 bones_from[32], bones_to[32];\n"
"\n"
"vec4 bone_lerp(vec4 from, vec4 to, float alpha) {\n"
"//Go around the short way\n"
"if (from.z + pi < to.z)\n"
"to.z -= 2*pi;\n"
"if (from.z - pi > to.z)\n"
"to.z += 2*pi;\n"
"return from * (1. - alpha) + to * alpha;\n"
"}\n"
"\n"
"vec2 skinned_pos_at(float alpha) {\n"
"vec4 eff_bone = bone_lerp(bones_from[bone], bones_to[bone], alpha);\n"
"vec4 eff_parent = bone_lerp(bones_from[parent], bones_to[parent], alpha);\n"
"\n"
"//Interpolate the rotation by 'weight', then translate with the lower bone\n"
"float rot = eff_parent.z + (eff_bone.z - eff_parent.z) * weight;\n"
"float y_scale = eff_parent.w + (eff_bone.w - eff_parent.w) * weight;\n"
"vec2 scale = vec2(1./y_scale, y_scale);\n"
"vec2 bone_rel_pos = rotation(-rot) * (position * scale);\n"
"return eff_bone.xy + bone_rel_pos;\n"
"}\n"
"\n"
"vec2 skinned_pos() {\n"
"return skinned_pos_at(alpha);\n"
"}\n"
"\n"
"in float blur_alpha;\n"
"uniform float ground_speed;\n"
"\n"
"vec3 blur_skinned_pos(float threshold, float exaggeration) {\n"
"float inv_ba = 1. - blur_alpha;\n"
"vec2 offset = vec2(-1, 0)*ground_speed*inv_ba;\n"
"vec2 start_pos = skinned_pos_at(0);\n"
"vec2 end_pos = skinned_pos_at(1) + offset;\n"
"if (distance(start_pos, end_pos) > threshold)\n"
"return vec3(skinned_pos_at(alpha - inv_ba*exaggeration) + offset*exaggeration, inv_ba);\n"
"else\n"
"return vec3(skinned_pos_at(alpha), 0.);\n"
"}\n"
"\n"
"vec3 blur_skinned_pos() {\n"
"return blur_skinned_pos(.05, .4);\n"
"}\n"
"\n"
"out float frag_blur_alpha;\n"
"\n"
"void set_frag_blur_input() {\n"
"frag_blur_alpha = blur_alpha;\n"
"}\n"
"\n"
"uniform mat3 camera;\n"
"uniform mat3 transform;\n"
"\n"
"out vec2 posFrag;\n"
"\n"
"void output_local_world_space(vec2 pos) {\n"
"gl_Position = vec4((camera * transform * vec3(pos, 1)).xy, 0, 1);\n"
"posFrag = (transform * vec3(pos, 1)).xy;\n"
"}\n"
"\n"
"void output_local_world_space(vec3 pos) {\n"
"gl_Position = vec4((camera * transform * vec3(pos.xy, 1)).xy, pos.z, 1);\n"
"posFrag = (transform * vec3(pos.xy, 1)).xy;\n"
"}\n"
};
shader_t noise_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/noise.frag",
#endif
.name = "noise_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform float time;\n"
"float snoise(vec3 v);\n"
"float snoise(vec2 v);\n"
"float srdnoise(in vec2 P, in float rot, out vec2 grad);\n"
"in vec2 posFrag;\n"
"\n"
"void main()\n"
"{\n"
"float pulse = pow(sin(time*7), 6);\n"
"vec2 grad, grad1;\n"
"float noise = srdnoise(posFrag*.3, time*1.5, grad);\n"
"float noise0 = srdnoise(posFrag*.005, time*.2, grad1);\n"
"float noise1 = max(noise0 - .1 + pulse*.1, 0) * (pulse*.5+1);\n"
"color = vec4(grad * noise1, noise * max(noise0-.35, 0)*100., 0) * .5 + .5;\n"
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
"int seed = rand(gl_InstanceID) ^ int(1/fract(transform[0,2] + transform[1,2]));\n"
"int angle = rand(seed);\n"
"int speed = rand(angle);\n"
"\n"
"float theta = float(angle) / pow(2., 20);\n"
"vec2 dir = vec2(cos(theta), sin(theta));\n"
"vec2 vel = dir * (float(speed) / pow(2., 32)) + init_vel + vec2(0, -time);\n"
"\n"
"vec2 pos = position*vec2(.05,.002);\n"
"vec2 vel_dir = normalize(vel);\n"
"pos = mat2(vel_dir.x, vel_dir.y, -vel_dir.y, vel_dir.x) * pos;\n"
"\n"
"pos += time * (vel);\n"
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
shader_t stickman_blur_frag = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/stickman_blur.frag",
#endif
.name = "stickman_blur_frag",
.type = GL_FRAGMENT_SHADER,
.source =
"out vec4 color;\n"
"uniform sampler2DRect framebuffer;\n"
"in float frag_blur_alpha;\n"
"uniform bool attacking;\n"
"\n"
"void main()\n"
"{\n"
"color = texture(framebuffer, gl_FragCoord.xy);\n"
"if (attacking)\n"
"color += frag_blur_alpha*vec4(.9, .6, .7, 1);\n"
"else\n"
"color += vec4(frag_blur_alpha);\n"
"}\n"
};
shader_t stickman_blur_vert = &(struct shader){
.shader = 0,
#ifdef DEBUG
.fname = "/Users/dan/Projects/Fighting_Game/Fighting Game/shaders/stickman_blur.vert",
#endif
.name = "stickman_blur_vert",
.type = GL_VERTEX_SHADER,
.source =
"vec3 blur_skinned_pos();\n"
"void output_local_world_space(vec3 pos);\n"
"void set_frag_blur_input();\n"
"vec3 blur_skinned_pos(float threshold, float exaggeration);\n"
"\n"
"uniform bool attacking;\n"
"\n"
"void main()\n"
"{\n"
"set_frag_blur_input();\n"
"if (attacking)\n"
"output_local_world_space(blur_skinned_pos(0.1, 1.5));\n"
"else\n"
"output_local_world_space(blur_skinned_pos());\n"
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
