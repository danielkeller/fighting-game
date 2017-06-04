out vec4 color;
uniform sampler2DRect framebuffer;
in float frag_blur_alpha;

void main()
{
    color = texture(framebuffer, gl_FragCoord.xy);
    color += vec4(frag_blur_alpha);//*vec4(.7, .8, .9, 1);
}
