vec2 skinned_pos();
void output_local_world_space(vec2 pos);

void main()
{
    output_local_world_space(skinned_pos());
}
