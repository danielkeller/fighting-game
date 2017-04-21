out vec4 color;
uniform float direction;
uniform float health, last_health, time_since_last_change;

in vec2 uv;

void main()
{
    if (uv.x < health/100.)
        color = vec4(1, 0, 0, 1);
    else if (uv.x < last_health/100.) {
        color = vec4(1, 1, 1, 1) * exp(-3.*time_since_last_change);
    }
    else
        color = vec4(0, 0, 0, 0);
}
