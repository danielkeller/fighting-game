in vec2 position;
out vec2 posFrag;

void main()
{
    gl_Position = vec4(position*2-1, 1, 1);
    //posFrag is supposed to be in world space
    posFrag = position*2-1;
}
