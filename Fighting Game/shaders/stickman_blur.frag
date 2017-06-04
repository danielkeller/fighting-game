out vec4 color;
uniform sampler2DRect framebuffer;
in float frag_blur_alpha;
uniform bool attacking;

void main()
{
    color = texture(framebuffer, gl_FragCoord.xy);
    if (attacking)
        color += frag_blur_alpha*vec4(.9, .6, .7, 1);
    else
        color += vec4(frag_blur_alpha);
}
