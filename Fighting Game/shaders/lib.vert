const float pi = 3.1415926535897;

mat2 rotation(float t) {
    return mat2(cos(t), -sin(t),
                sin(t), cos(t));
}

in vec2 position;
in int bone, parent;
in float weight;
uniform float alpha;
uniform vec4 bones_from[32], bones_to[32];

vec4 bone_lerp(vec4 from, vec4 to, float alpha) {
    //Go around the short way
    if (from.z + pi < to.z)
        to.z -= 2*pi;
    if (from.z - pi > to.z)
        to.z += 2*pi;
    return from * (1. - alpha) + to * alpha;
}

vec2 skinned_pos_at(float alpha) {
    vec4 eff_bone = bone_lerp(bones_from[bone], bones_to[bone], alpha);
    vec4 eff_parent = bone_lerp(bones_from[parent], bones_to[parent], alpha);
    
    //Interpolate the rotation by 'weight', then translate with the lower bone
    float rot = eff_parent.z + (eff_bone.z - eff_parent.z) * weight;
    float y_scale = eff_parent.w + (eff_bone.w - eff_parent.w) * weight;
    vec2 scale = vec2(1./y_scale, y_scale);
    vec2 bone_rel_pos = rotation(-rot) * (position * scale);
    return eff_bone.xy + bone_rel_pos;
}

vec2 skinned_pos() {
    return skinned_pos_at(alpha);
}

in float blur_alpha;
uniform float ground_speed;

vec3 blur_skinned_pos(float threshold, float exaggeration) {
    float inv_ba = 1. - blur_alpha;
    vec2 offset = vec2(-1, 0)*ground_speed*inv_ba;
    vec2 start_pos = skinned_pos_at(0);
    vec2 end_pos = skinned_pos_at(1) + offset;
    if (distance(start_pos, end_pos) > threshold)
        return vec3(skinned_pos_at(alpha - inv_ba*exaggeration) + offset*exaggeration, inv_ba);
    else
        return vec3(skinned_pos_at(alpha), 0.);
}

vec3 blur_skinned_pos() {
    return blur_skinned_pos(.05, .4);
}

out float frag_blur_alpha;

void set_frag_blur_input() {
    frag_blur_alpha = blur_alpha;
}

uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;

void output_local_world_space(vec2 pos) {
    gl_Position = vec4((camera * transform * vec3(pos, 1)).xy, 0, 1);
    posFrag = (transform * vec3(pos, 1)).xy;
}

void output_local_world_space(vec3 pos) {
    gl_Position = vec4((camera * transform * vec3(pos.xy, 1)).xy, pos.z, 1);
    posFrag = (transform * vec3(pos.xy, 1)).xy;
}
