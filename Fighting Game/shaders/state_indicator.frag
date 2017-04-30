out vec4 color;
in vec2 uv;

uniform int top, bottom, top_attack, bottom_attack;

void main()
{
    if (uv.y > .5)
    {
        if (uv.x > .5)
            color = vec4(1,0,0,1)*float(top_attack)*.33;
        else
            color = vec4(1,1,1,1)*float(top)*.33;
    }
    else
    {
        if (uv.x > .5)
            color = vec4(1,0,0,1)*float(bottom_attack)*.33;
        else
            color = vec4(1,1,1,1)*float(bottom)*.33;
    }
}
