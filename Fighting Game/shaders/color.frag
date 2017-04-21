out vec4 color;
uniform vec3 main_color;
uniform sampler2D framebuffer;

void main()
{
    color = vec4(main_color, 1);
}
