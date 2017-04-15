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

enum {
    idle, base_mid, mid_bot
};

static const state_t states[] = {
    [idle] = {1, &stickman_Basis_Basis_Basis,
        {.balance = 10, .hi = {8, 10}, .mid = {2, 3}}},
    [base_mid] = {2, &stickman_swing_Basis_DS_Mid,
        {.balance = 6, .hi = {10, 16}, .mid = {1, 1}}},
    [mid_bot] = {3, &stickman_swing_DS_Mid_DS_Bot,
        {.balance = 2, .hi = {1, 2}, .mid = {1, 1}}},
};

static const float speed = .03;
static const float hitbox_width = .2;
static const int attack_buff = 2;
static const int defense_buff = 2;

void goto_state(stickman_t *sm, long long frame, int state)
{
    sm->next.state = state;
    sm->next.fight_state = states[state].fight_state;
    sm->anim_start = frame;
    anim_obj_keys(&sm->obj, states[state].anim);
}

void next_state(stickman_t *sm, long long frame, int state)
{
    if (frame - sm->anim_start >= states[sm->prev.state].frames)
        goto_state(sm, frame, state);
}

void run_anim(stickman_t *sm, long long frame, float alpha)
{
    //next state determines which animation is drawn
    float anim_alpha = ((float)(frame - sm->anim_start) + alpha)
        / (float)states[sm->next.state].frames;
    glUniform1f(sm->program.pos_alpha, anim_alpha);
}

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
    glUniform3f(sm->color_unif, 1., 1., 1.);
    
    sm->prev = sm->next = (stickman_state_t){
        .state = 0,
        .advancing = 0,
        .ground_pos = -1.f,
        .health = 100,
        .fight_state = states[idle].fight_state
    };
    
    sm->direction = direction;
    sm->other = other;
}

//State change forcing

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
//Also, 'frame' is the number of the previous frame.
void stickman_actions(stickman_t* sm, long long frame, int advance, int attack)
{
    sm->prev = sm->next;
    
    switch (sm->prev.state) {
        case 0:
            if (attack)
                goto_state(sm, frame, base_mid);
            break;
        case 1:
            next_state(sm, frame, mid_bot);
            break;
        case 2:
            next_state(sm, frame, idle);
            break;
        default:
            break;
    }
    
    float other_pos = -sm->other->next.ground_pos;
    float fwd_limit = fmin(1, other_pos) - hitbox_width*2.;
    
    if (advance && sm->prev.ground_pos != fwd_limit)
        sm->next.ground_pos = fmin(fwd_limit, sm->prev.ground_pos + speed);
    else if (!advance && sm->prev.ground_pos != -1.)
        sm->next.ground_pos = fmax(-1, sm->prev.ground_pos - speed);

    sm->next.advancing = advance;
}

//The attack is resolved against the previous state. This
//is so the character which is updated first doesn't have
//an advantage.
void resolve_attack(stickman_t* attacker, strike_point_t* target, attack_t* attack, long long frame)
{
    stickman_t* victim = attacker->other;
    
    int effective_momentum =
        attack->momentum + attacker->prev.advancing*attack_buff
        - target->momentum - (!victim->prev.advancing)*defense_buff;
    if (effective_momentum >= attack->momentum / 2) //Attack lands
    {
        int damage = attack->damage - target->defense;
        victim->next.health -= damage;
        if (victim->next.health < 0)
            victim->next.health = 0;
    }
    int knockback = effective_momentum - victim->prev.fight_state.balance;
    if (knockback > 0)
    {
        victim->next.advancing = 0;
        goto_state(victim, frame, idle);
        victim->next.ground_pos -= .02*knockback;
    }
}

void attack(stickman_t* sm, long long frame, attack_t attack)
{
    if (sm->prev.state == attack.state
        && frame - sm->anim_start == attack.frame
        && -sm->other->prev.ground_pos - sm->prev.ground_pos < attack.range)
        resolve_attack(sm, &sm->other->prev.fight_state.hi + attack.target, &attack, frame);
}

#define T(target) (&((fight_state_t*)NULL)->target - &((fight_state_t*)NULL)->hi)

void stickman_consequences(stickman_t* sm, long long frame)
{
    //state, frame, target, range, damage, momentum
    attack(sm, frame, (attack_t){1, 1, T(hi), .6, 20, 20});
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
    
    run_anim(sm, frame, alpha);
    
    glDrawArrays(GL_TRIANGLES, 0, sm->obj.numVertecies);
    
    draw_health_bar(&sm->health_bar, sm->prev.health);
}

void free_stickman(stickman_t* sm)
{
    free_health_bar(&sm->health_bar);
    free_object(&sm->obj);
    free_program(&sm->program);
}
