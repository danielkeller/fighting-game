out vec4 color;
uniform vec3 main_color;

uniform sampler2D framebuffer;

in vec2 posFrag;
in vec2 framebuffer_coord;

void main()
{
    color = vec4(main_color, 1);
}
