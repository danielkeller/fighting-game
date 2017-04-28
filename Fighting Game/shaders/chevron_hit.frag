out vec4 color;
uniform float time, direction;
uniform vec2 origin;

uniform sampler2DRect framebuffer;

in vec2 posFrag;

float snoise(vec2 v);
float srdnoise(in vec2 P, in float rot, out vec2 grad);

void main()
{
    float move_amt = time*.5;
    const float xslope = 1, yslope = 1;
    
    vec2 dir = posFrag - origin;
    vec2 moved_dir = dir - vec2(direction*move_amt, 0);
    float dist = direction*dir.x + abs(dir.y);
    float moved_dist = direction*moved_dir.x + abs(moved_dir.y);
    vec2 grad = vec2(direction*xslope, yslope);
    
    float amount = mod(moved_dist*2, 1);
    vec2 displacement = exp(amount*3)*normalize(grad)*10;
    
    vec2 noisegrad;
    float noise = srdnoise(moved_dir*vec2(10,100), move_amt, noisegrad)*.2 + .3;
    vec4 draw_color = vec4(0);
    if (amount > .8)
        draw_color = vec4((dist+1)*noise,0,0,1);
    else if (amount < .1)
        draw_color = vec4(.1-amount, 0, 0, .1-amount)*4*(dist+1);
    
    color = texture(framebuffer, gl_FragCoord.xy - displacement);
    color = color*(1-draw_color.a) + draw_color;
}
