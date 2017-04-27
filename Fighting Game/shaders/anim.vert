in vec2 pos_from, deriv_from, pos_to, deriv_to;
uniform float pos_alpha;

uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;

vec2 spline(vec2 a, vec2 da, vec2 b, vec2 db, float alpha);

void main()
{
    vec2 position = spline(pos_from, deriv_from, pos_to, deriv_to, pos_alpha);
    gl_Position = vec4((camera * transform * vec3(position, 1)).xy, 0, 1);
    posFrag = (transform * vec3(position, 1)).xy;
}
