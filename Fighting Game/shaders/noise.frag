out vec4 color;
uniform float time;
float snoise(vec3 v);
float snoise(vec2 v);
float srdnoise(in vec2 P, in float rot, out vec2 grad);
in vec2 posFrag;

void main()
{
    float pulse = pow(sin(time*7), 6);
    vec2 grad, grad1;
    float noise = srdnoise(posFrag*100., time*1.5, grad);
    float noise0 = srdnoise(posFrag*3., time*.2, grad1);
    float noise1 = max(noise0 - .1 + pulse*.1, 0) * (pulse*.5+1);
    color = vec4(grad * noise1, noise * max(noise0-.35, 0)*100., 0) * .5 + .5;
}
