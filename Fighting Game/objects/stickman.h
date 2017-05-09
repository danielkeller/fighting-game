#include "types.h"
extern const mesh_t stickman_legs_mesh;
extern const struct anim_step stickman_legs_anims[];
enum stickman_legs_state_names {
    walk, 
};
extern const mesh_t stickman_torso_mesh;
extern const struct anim_step stickman_torso_anims[];
enum stickman_torso_state_names {
    hi_unblock, 
    bottom, 
    top, 
    swing_1, swing_2, 
    lo_unblock_1, lo_unblock_2, lo_unblock_3, 
    block, 
    hi_block, 
    lo_block_1, lo_block_2, lo_block_3, 
    swingup_1, swingup_2, 
};
