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

static const int base_mid_frames = 3;
static const int mid_bot_frames = 4;

void make_stickman(stickman_t *sm)
{
    make_anim_obj(&sm->obj, stickman_verts, sizeof(stickman_verts), stickman_stride);
    anim_obj_keys(&sm->obj, stickman_Basis, stickman_Basis);
    load_shader_program(&sm->program, "assets/anim.vert", "assets/color.frag");
    sm->color_unif = glGetUniformLocation(sm->program.program, "main_color");
    sm->state = 0;
}

void update_stickman(stickman_t* sm, long long frame, int attack)
{
    switch (sm->state) {
        case 0:
            if (attack)
            {
                sm->state = 1;
                sm->anim_start = frame;
                anim_obj_keys(&sm->obj, stickman_Basis, stickman_DS_Mid);
            }
            break;
        case 1:
            if (frame - sm->anim_start >= base_mid_frames)
            {
                sm->state = 2;
                sm->anim_start = frame;
                anim_obj_keys(&sm->obj, stickman_DS_Mid, stickman_DS_Bot);
            }
            break;
        case 2:
            if (frame - sm->anim_start >= mid_bot_frames)
            {
                sm->state = 0;
                anim_obj_keys(&sm->obj, stickman_Basis, stickman_Basis);
            }
            break;
        default:
            break;
    }
}

void draw_stickman(stickman_t* sm, long long frame, float alpha)
{
    glUseProgram(sm->program.program);
    glUniformMatrix3fv(sm->program.camera, 1, GL_FALSE, camera);
    glUniformMatrix3fv(sm->program.transform, 1, GL_FALSE, eye3);
    glUniform3f(sm->color_unif, 1., 1., 1.);
    glBindVertexArray(sm->obj.vertexArrayObject);
    
    switch (sm->state) {
        case 0:
            break;
        case 1:
        {
            float anim_alpha = ((float)(frame - sm->anim_start) + alpha)
                / (float)base_mid_frames;
            glUniform1f(sm->program.pos_alpha, anim_alpha);
            break;
        }
        case 2:
        {
            float anim_alpha = ((float)(frame - sm->anim_start) + alpha)
            / (float)mid_bot_frames;
            glUniform1f(sm->program.pos_alpha, anim_alpha);
            break;
        }
        default:
            break;
    }
    
    glDrawArrays(GL_TRIANGLES, 0, sm->obj.numVertecies);
}

void free_stickman(stickman_t* sm)
{
    free_object(&sm->obj);
}