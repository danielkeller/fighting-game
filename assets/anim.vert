in vec2 pos_from;
in vec2 pos_to;
uniform float pos_alpha;

uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;
out vec2 framebuffer_coord;

void main()
{
    vec2 position = pos_from*(1. - pos_alpha) + pos_to*pos_alpha;
    gl_Position = vec4((camera * transform * vec3(position, 1)).xy, 0, 1);
    posFrag = (transform * vec3(position, 1)).xy;
    
    framebuffer_coord = gl_Position.xy / 2. + 0.5;
}
