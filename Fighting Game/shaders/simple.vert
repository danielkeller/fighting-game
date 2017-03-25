in vec2 position;

uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;
out vec2 framebuffer_coord;

void main()
{
    gl_Position = vec4(
        (camera * transform * vec3(position, 1)).xy, 0, 1);
    posFrag = (transform * vec3(position, 1)).xy;
    
    framebuffer_coord = gl_Position.xy / 2. + 0.5;
}
