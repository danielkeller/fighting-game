in vec2 position;
in int bone, parent;
in float weight;
uniform float alpha;
uniform vec2 bones[32];
uniform vec4 bones_from[32], bones_to[32];

mat2 rotation(float t) {
    return mat2(cos(t), -sin(t),
                sin(t), cos(t));
}

vec2 skinned_pos() {
    vec4 eff_bone = bones_from[bone] * (1. - alpha) + bones_to[bone] * alpha;
    vec4 eff_parent = bones_from[parent] * (1. - alpha) + bones_to[parent] * alpha;
    
    float rel_bone_rot = (eff_bone.z - eff_parent.z) * (1 - weight);
    float rot = eff_bone.z - rel_bone_rot;
    vec2 bone_rel_pos = position - bones[bone];
    bone_rel_pos = rotation(rot) * bone_rel_pos;
    return eff_bone.xy + bone_rel_pos;
}

vec2 spline(vec2 a, vec2 da, vec2 b, vec2 db, float alpha)
{
    vec2 x = da - (b - a),
    y = -db + (b - a);
    float inv_alpha = 1. - alpha;
    return a*inv_alpha + b*alpha + inv_alpha*alpha*(x*inv_alpha + y*alpha);
}

vec2 spline(vec2 a, vec2 da, vec2 b, vec2 db, float alpha, out vec2 deriv)
{
    vec2 x = da - (b - a),
    y = -db + (b - a);
    float inv_alpha = 1. - alpha,
        d_alpha1 = 2. - 3.*alpha,
        d_alpha2 = 1. - 2.*alpha;
    
    deriv = b - a + d_alpha2*x + d_alpha1*(y*alpha - x*alpha);
    return a*inv_alpha + b*alpha + inv_alpha*alpha*(x*inv_alpha + y*alpha);
}
