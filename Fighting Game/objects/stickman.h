#include "types.h"
typedef struct anim_step anim_step_t;
static const GLsizei stickman_stride = 208;
extern const anim_step_t stickman_anims[];
enum state_names {
bottom, 
top, 
swing_1, swing_2, 
unblock, 
block, 
hi_block, 
lo_block_1, lo_block_2, lo_block_3, 
swingup_1, swingup_2, 
};
extern float stickman_verts[4212];
