//
//  stickman.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/1/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "stickman.h"
#include "objects/stickman.h"
#include "shaders.h"
#include "gl_core_3_3.h"
#include "world.h"
#include "math.h"
#include <math.h>
#include <stdlib.h>

static const animation_t
//Name      id  from             to               frames
idle =     {0,  &stickman_Basis_Basis_Basis,  1},
base_mid = {1,  &stickman_swing_Basis_DS_Mid, 2},
mid_bot =  {2,  &stickman_swing_DS_Mid_DS_Bot, 3};

static const float speed = .03;
static const float hitbox_width = .2;

#define START_ANIM(anim) \
    do { \
    sm->next.state = anim.id; \
    sm->anim_start = frame; \
    anim_obj_keys(&sm->obj, anim.step); \
    } while (0)

#define NEXT_ANIM(cur, anim) \
    do { \
    if (frame - sm->anim_start >= cur.frames) { \
        sm->next.state = anim.id; \
        sm->anim_start = frame; \
        anim_obj_keys(&sm->obj, anim.step); \
    } \
    } while (0)

#define RUN_ANIM(anim) \
    do { \
    float anim_alpha = ((float)(frame - sm->anim_start) + alpha) \
        / (float)anim.frames; \
    glUniform1f(sm->program.pos_alpha, anim_alpha); \
    } while (0)

void make_heath_bar(health_bar_t* hb, direction_t direction)
{
    make_box(&hb->obj);
    load_shader_program(&hb->program, health_bar_vert, color_frag);
    hb->health_unif = glGetUniformLocation(hb->program.program, "health");
    
    glUseProgram(hb->program.program);
    GLint color_unif = glGetUniformLocation(hb->program.program, "main_color");
    glUniform3f(color_unif, 1., 0., 0.);
    GLint direction_unif = glGetUniformLocation(hb->program.program, "direction");
    glUniform1f(direction_unif, (float)direction);
}

void draw_health_bar(health_bar_t* hb, int health)
{
    glUseProgram(hb->program.program);
    glUniform1f(hb->health_unif, (float)health);
    glBindVertexArray(hb->obj.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, hb->obj.numVertecies);
}

void free_health_bar(health_bar_t* hb)
{
    free_object(&hb->obj);
    free_program(&hb->program);
}

void make_stickman(stickman_t *sm, stickman_t* other, direction_t direction)
{
    make_heath_bar(&sm->health_bar, direction);
    make_anim_obj(&sm->obj, stickman_verts, sizeof(stickman_verts), stickman_stride);
    anim_obj_keys(&sm->obj, &stickman_Basis_Basis_Basis);
    load_shader_program(&sm->program, anim_vert, color_frag);
    sm->color_unif = glGetUniformLocation(sm->program.program, "main_color");
    
    sm->prev = sm->next = (stickman_state_t){
        .state = 0,
        .advancing = STATIONARY,
        .ground_pos = -1.f,
    };
    
    sm->health = 100;
    sm->direction = direction;
    sm->other = other;
}


//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame.
//Also, 'frame' is the number of the previous frame.
void update_stickman(stickman_t* sm, stickman_t* enemy,
                     long long frame, int advance, int attack)
{
    sm->prev = sm->next;
    
    switch (sm->prev.state) {
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
    
    float other_pos = -sm->other->next.ground_pos;
    float fwd_limit = fmin(1, other_pos) - hitbox_width;
    
    if (advance && sm->prev.ground_pos != fwd_limit) {
        sm->next.advancing = ADVANCING;
        sm->next.ground_pos = fmin(fwd_limit, sm->prev.ground_pos + speed);
    }
    else if (!advance && sm->prev.ground_pos != -1.) {
        sm->next.advancing = RETREATING;
        sm->next.ground_pos = fmax(-1, sm->prev.ground_pos - speed);
    }
    else
        sm->next.advancing = STATIONARY;
}

//'frame' is also the number of the previous frame here.
void draw_stickman(stickman_t* sm, long long frame, float alpha)
{
    glUseProgram(sm->program.program);
    glBindVertexArray(sm->obj.vertexArrayObject);
    
    glUniformMatrix3fv(sm->program.camera, 1, GL_FALSE, camera.d);
    
    float ground_pos = sm->prev.ground_pos * (1. - alpha)
                     + sm->next.ground_pos * alpha;
    
    Mat3 pos = affine(0., ground_pos * sm->direction, 0.);
    pos.d[0] = sm->direction;
    glUniformMatrix3fv(sm->program.transform, 1, GL_FALSE, pos.d);
    glUniform3f(sm->color_unif, 1., 1., 1.);
    
    switch (sm->next.state) {
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
    
    draw_health_bar(&sm->health_bar, sm->health);
}

void free_stickman(stickman_t* sm)
{
    free_health_bar(&sm->health_bar);
    free_object(&sm->obj);
    free_program(&sm->program);
}
