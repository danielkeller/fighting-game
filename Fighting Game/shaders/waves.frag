out vec4 color;
uniform float time;
uniform vec2 origin;
uniform vec3 main_color;
uniform vec3 lead_color;

uniform sampler2D framebuffer;

in vec2 posFrag;
in vec2 framebuffer_coord;

void main()
{
    float wave = distance(posFrag, origin)*5 - time*.6;
    float pulse = 1;// - abs(cos(time*16)) * .2;
    float val = pow(mod(wave, 1.), 2 * pulse) + .02;// - .3*rand(posFrag + time);
    
    float val1 = 0.;//pow(1 - mod(wave, 1.), 16);
    
    vec4 dest = texture(framebuffer, framebuffer_coord);
    float src_a = val + val1;
    vec3 src = (main_color*val + lead_color*val1)/src_a;
    float extra_a = max(dest.a - src_a, 0.);
    src_a = max(src_a - extra_a, 0.);
    color = dest * (1 - src_a) + vec4(src*src_a, src_a);
}
