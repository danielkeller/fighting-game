#include "types.h"
static const GLsizei stickman_stride = 248;
extern const struct anim_step stickman_anims[];
enum state_names {
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
extern float stickman_verts[5022];
