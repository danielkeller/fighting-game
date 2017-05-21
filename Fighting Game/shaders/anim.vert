uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;

vec2 skinned_pos();

void main()
{
    vec2 pos = skinned_pos();
    gl_Position = vec4((camera * transform * vec3(pos, 1)).xy, 0, 1);
    posFrag = (transform * vec3(pos, 1)).xy;
}
