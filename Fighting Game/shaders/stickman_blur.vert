vec3 blur_skinned_pos();
void output_local_world_space(vec3 pos);
void set_frag_blur_input();
vec3 blur_skinned_pos(float threshold, float exaggeration);

uniform bool attacking;

void main()
{
    set_frag_blur_input();
    if (attacking)
        output_local_world_space(blur_skinned_pos(0.1, 1.5));
    else
        output_local_world_space(blur_skinned_pos());
}
