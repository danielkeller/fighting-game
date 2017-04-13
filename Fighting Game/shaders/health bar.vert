in vec2 position;

uniform float direction;
uniform float health;

out vec2 posFrag;
out vec2 framebuffer_coord;

void main()
{
    mat3 transform = mat3(
        .8*direction*(health / 100.),
            0, 0,
        0, .1, 0,
        .1*direction, .8, 1);
    
    gl_Position = vec4((transform * vec3(position, 1)).xy, 0, 1);
    posFrag = (transform * vec3(position, 1)).xy;
    
    framebuffer_coord = gl_Position.xy / 2. + 0.5;
}
