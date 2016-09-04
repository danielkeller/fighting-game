//
//  stickman.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/1/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "stickman.h"
#include "objects/stickman.h"
#include "gl_core_3_3.h"
#include "world.h"
#include "math.h"
#include <math.h>

static animation_t
//Name      id  from             to               frames
idle =     {0,  stickman_Basis,  stickman_Basis,  1},
base_mid = {1,  stickman_Basis,  stickman_DS_Mid, 2},
mid_bot =  {2,  stickman_DS_Mid, stickman_DS_Bot, 3};

static float speed = .1;

#define START_ANIM(anim) \
    do { \
    sm->state = anim.id; \
    sm->anim_start = frame; \
    anim_obj_keys(&sm->obj, anim.from, anim.to); \
    } while (0)

#define NEXT_ANIM(cur, next) \
    do { \
    if (frame - sm->anim_start >= cur.frames) { \
        sm->state = next.id; \
        sm->anim_start = frame; \
        anim_obj_keys(&sm->obj, next.from, next.to); \
    } \
    } while (0)

#define RUN_ANIM(anim) \
    do { \
    float anim_alpha = ((float)(frame - sm->anim_start) + alpha) \
        / (float)anim.frames; \
    glUniform1f(sm->program.pos_alpha, anim_alpha); \
    } while (0)

void make_stickman(stickman_t *sm)
{
    make_anim_obj(&sm->obj, stickman_verts, sizeof(stickman_verts), stickman_stride);
    anim_obj_keys(&sm->obj, stickman_Basis, stickman_Basis);
    load_shader_program(&sm->program, "assets/anim.vert", "assets/color.frag");
    sm->color_unif = glGetUniformLocation(sm->program.program, "main_color");
    sm->state = 0;
    sm->advancing = STATIONARY;
    sm->ground_pos = 1.f;
}

void update_stickman(stickman_t* sm, long long frame, int advance, int attack)
{
    switch (sm->state) {
        case 0:
            if (attack)
                START_ANIM(base_mid);
            break;
        case 1:
            NEXT_ANIM(base_mid, mid_bot);
            break;
        case 2:
            NEXT_ANIM(mid_bot, idle);
            break;
        default:
            break;
    }
    
    if (advance && sm->advancing == ADVANCING)
        sm->ground_pos = fmax(0., sm->ground_pos - speed);
    else if (!advance && sm->advancing == RETREATING)
        sm->ground_pos = fmin(1., sm->ground_pos + speed);
    
    if (advance && sm->ground_pos != 0.)
        sm->advancing = ADVANCING;
    else if (!advance && sm->ground_pos != 1.)
        sm->advancing = RETREATING;
    else
        sm->advancing = STATIONARY;
}

void draw_stickman(stickman_t* sm, long long frame, float alpha)
{
    glUseProgram(sm->program.program);
    glBindVertexArray(sm->obj.vertexArrayObject);
    
    glUniformMatrix3fv(sm->program.camera, 1, GL_FALSE, camera.d);
    
    float ground_pos = sm->ground_pos;
    if (sm->advancing == ADVANCING)
        ground_pos -= speed*alpha;
    if (sm->advancing == RETREATING)
        ground_pos += speed*alpha;
    ground_pos = fmin(1, fmax(0, ground_pos));
    
    Mat3 pos = affine(0., ground_pos, 0.);
    glUniformMatrix3fv(sm->program.transform, 1, GL_FALSE, pos.d);
    glUniform3f(sm->color_unif, 1., 1., 1.);
    
    switch (sm->state) {
        case 0:
            break;
        case 1:
            RUN_ANIM(base_mid);
            break;
        case 2:
            RUN_ANIM(mid_bot);
            break;
        default:
            break;
    }
    
    glDrawArrays(GL_TRIANGLES, 0, sm->obj.numVertecies);
}

void free_stickman(stickman_t* sm)
{
    free_object(&sm->obj);
}