out vec4 color;
uniform float time;
uniform vec2 origin;

uniform sampler2DRect framebuffer;

in vec2 posFrag;

void main()
{
    vec2 dir = posFrag - origin;
    float amount = mod(sqrt(length(dir))*10 - time*4, 1);
    vec2 displacement = amount*normalize(dir)*100;
    
    color = texture(framebuffer, gl_FragCoord.xy + displacement);
}
