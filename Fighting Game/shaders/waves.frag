out vec4 color;
uniform float time;
uniform float intensity;

uniform sampler2DRect framebuffer;

in vec2 posFrag;

void main()
{
    vec2 origin = vec2(0, 1);
    vec3 main_color = vec3(1, 0, 0);
    vec3 lead_color = vec3(1, 1, 0);
    
    float wave = distance(posFrag, origin)*5 - time*.6;
    float pulse = 1 - pow(sin(time*7), 6) * .2;
    float val = pow(mod(wave, 1.), pulse*2)*.6*intensity + .02;// - .3*rand(posFrag + time);
    
    float val1 = 0.;//pow(1 - mod(wave, 1.), 16);
    
    vec4 dest = texture(framebuffer, gl_FragCoord.xy);
    float src_a = val + val1;
    vec3 src = (main_color*val + lead_color*val1)/src_a;
    float extra_a = max(dest.a - src_a, 0.);
    src_a = max(src_a - extra_a, 0.);
    color = dest * (1 - src_a) + vec4(src*src_a, src_a);
}
