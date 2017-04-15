out vec4 color;

uniform sampler2DRect framebuffer;

in vec2 posFrag;

void main()
{
    color = texture(framebuffer, gl_FragCoord.xy);
}
