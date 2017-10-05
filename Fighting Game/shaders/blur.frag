out vec4 color;
uniform sampler2DRect framebuffer;
uniform sampler2DRect noise;
uniform float intensity;
uniform float time;
in vec2 posFrag;

void main()
{
    vec2 noise_coord = gl_FragCoord.xy / textureSize(framebuffer) * textureSize(noise);
    vec4 noise_val = texture(noise, noise_coord);
    color = texture(framebuffer, gl_FragCoord.xy + (noise_val.xy - .5) * 40 * intensity);
    
    float pulse = pow(sin(time*7), 6);
    color -= pow(max(length(posFrag) - .5 + intensity*.2 - .1*pulse*intensity, 0), 4);
    color = max(color, -.06) + noise_val.z*.2 * (intensity+.2);
}
