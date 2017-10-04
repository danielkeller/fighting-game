out vec4 color;
uniform float time;
float snoise(vec3 v);
float srdnoise(in vec2 P, in float rot, out vec2 grad);

void main()
{
    float pulse = pow(sin(time*7), 6);
    vec2 grad, grad1;
    float noise = srdnoise(gl_FragCoord.xy*.3, time*1.5, grad);
    float noise1 = max(srdnoise(gl_FragCoord.xy*.005, time*.2, grad1) - .1 + pulse*.4, 0) * (pulse+1);
    color = vec4(grad * noise1 * .5 + .5, noise*noise1, 0);
}
