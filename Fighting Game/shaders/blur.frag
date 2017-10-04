out vec4 color;
uniform sampler2DRect framebuffer;
uniform sampler2DRect noise;
in vec2 posFrag;

void main()
{
    vec2 noise_coord = gl_FragCoord.xy / textureSize(framebuffer) * textureSize(noise);
    vec4 noise_val = texture(noise, noise_coord);
    color = texture(framebuffer, gl_FragCoord.xy + (noise_val.xy - .5) * 20);
}
