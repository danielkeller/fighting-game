out vec4 color;
uniform float time;
uniform float intensity;
float snoise(vec3 v);
float snoise(vec2 v);
float srdnoise(in vec2 P, in float rot, out vec2 grad);
in vec2 posFrag;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    float pulse = pow(sin(time*7), 6);
    vec2 grad, grad1;
    float noise = srdnoise(posFrag*100., time*1.5, grad);
    float noise0 = srdnoise(posFrag*3., time*.2, grad1);
    float noise1 = max(noise0 - .1 + pulse*.1, .02) * (pulse*.5+1);
    float noise2 = rand(posFrag + time) * (pulse*.2 + .8);
    color = vec4(grad * noise1 * .5 + .5, noise2, 0);
}
