#include "types.h"
typedef struct anim_step anim_step_t;
static const GLsizei stickman_stride = 104;
extern const anim_step_t stickman_anims[];
enum state_names {
top, 
bottom, 
swingup_1, swingup_2, 
swing_1, swing_2, 
};
extern float stickman_verts[2106];
