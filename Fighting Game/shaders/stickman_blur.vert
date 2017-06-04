vec3 blur_skinned_pos();
void output_local_world_space(vec3 pos);
void set_frag_blur_input();

void main()
{
    set_frag_blur_input();
    output_local_world_space(blur_skinned_pos());
}
