in vec2 posFrag;
out vec4 color;
uniform vec3 main_color;
uniform sampler2D framebuffer;
uniform float time;

float snoise(vec2 v);

void main()
{
    //vec2 uv = gl_FragCoord.xy/3.;
    //float noise = snoise(uv) + snoise(uv/2.+time) + snoise(uv/4.);
    //color = vec4(vec3(noise*.2+.8), 1);
    color = vec4(main_color, 1);
}
