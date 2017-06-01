const float pi = 3.1415926535897;

in vec2 position;
in int bone, parent;
in float weight;
uniform float alpha;
uniform vec4 bones_from[32], bones_to[32];

mat2 rotation(float t) {
    return mat2(cos(t), -sin(t),
                sin(t), cos(t));
}

vec4 bone_lerp(vec4 from, vec4 to) {
    //Go around the short way
    if (from.z + pi < to.z)
        to.z -= 2*pi;
    if (from.z - pi > to.z)
        to.z += 2*pi;
    return from * (1. - alpha) + to * alpha;
}

vec2 skinned_pos() {
    vec4 eff_bone = bone_lerp(bones_from[bone], bones_to[bone]);
    vec4 eff_parent = bone_lerp(bones_from[parent], bones_to[parent]);
    
    //Interpolate the rotation by 'weight', then translate with the lower bone
    float rot = eff_parent.z + (eff_bone.z - eff_parent.z) * weight;
    vec2 bone_rel_pos = rotation(-rot) * position;
    return eff_bone.xy + bone_rel_pos;
}

uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;

void output_local_world_space(vec2 pos) {
    gl_Position = vec4((camera * transform * vec3(pos, 1)).xy, 0, 1);
    posFrag = (transform * vec3(pos, 1)).xy;
}
