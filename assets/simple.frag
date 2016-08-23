out vec4 color;
uniform float time;
uniform vec2 origin;
uniform vec3 main_color;
uniform vec3 lead_color;

in vec2 posFrag;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    float wave = distance(posFrag, origin)*5 - time*.6;
    float pulse = 1;// - abs(cos(time*16)) * .2;
    float val = pow(mod(wave, 1.), 2 * pulse) + .02;// - .3*rand(posFrag + time);
    
    float val1 = pow(1 - mod(wave, 1.), 16);
    
    color = vec4(main_color*val + lead_color*val1, val + val1);
    gl_FragDepth = color.a;
}
