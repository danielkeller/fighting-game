out vec4 color;
uniform float time, direction;
uniform vec2 origin;

uniform sampler2DRect framebuffer;

in vec2 posFrag;

void main()
{
    vec2 dir = posFrag - origin;
    float dist = dir.x;// + direction*abs(dir.y);
    
    float amount = dist*2;
    
    color = texture(framebuffer, gl_FragCoord.xy);
    if (amount > .8 && amount < 1)
        color += vec4(1,0,0,1);
}
