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
#include "globals.h"
#include "math.h"
#include "fbo.h"
#include "time.h"
#include "function.h"
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

void goto_state(stickman_t *sm, int state)
{
    sm->next.state = state;
    sm->next.fight_state = states[state].fight_state;
    sm->anim_start = game_time.frame;
    anim_obj_keys(&sm->obj, states[state].anim);
}

void next_state(stickman_t *sm, int state)
{
    if (game_time.frame - sm->anim_start >= states[sm->prev.state].frames)
        goto_state(sm, state);
}

void run_anim(stickman_t *sm)
{
    //next state determines which animation is drawn
    float anim_alpha = ((float)(game_time.frame - sm->anim_start) + game_time.alpha)
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
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void free_health_bar(health_bar_t* hb)
{
    free_object(&hb->obj);
    free_program(&hb->program);
}

typedef struct hit_effect {
    stickman_t* attacker;
    usec_t start_time;
    GLint origin_unif;
    float x;
} hit_effect_t;

int draw_hit_effect(hit_effect_t* he)
{
    usec_t elapsed = game_time.current_time - he->start_time;
    
    swap_fbo(&fbo);
    glUseProgram(he->attacker->hit_effect.program);
    glUniform1f(he->attacker->hit_effect.time, (float)elapsed / 1000000.f);
    glUniform2f(he->origin_unif, he->x, .4);
    
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
    
    return elapsed < 100000ll;
}

BINDABLE(draw_hit_effect, hit_effect_t)

bound_t make_hit_effect(stickman_t* sm)
{
    hit_effect_t he;
    he.attacker = sm;
    he.start_time = game_time.current_time;
    he.origin_unif = glGetUniformLocation(sm->hit_effect.program, "origin");
    he.x = (sm->next.ground_pos + .4)*sm->direction;
    return bind_draw_hit_effect(&he);
}

typedef struct parry_effect {
    stickman_t* attacker;
    usec_t start_time;
    GLint color_unif, direction_unif, init_vel_unif;
    Mat3 position;
} parry_effect_t;

int draw_parry_effect(parry_effect_t* pe)
{
    program_t* program = &pe->attacker->parry_effect;
    usec_t elapsed = game_time.current_time - pe->start_time;
    glUseProgram(program->program);
    
    glUniform3f(pe->color_unif, 1., 1., 0.5);
    float dir = (float)pe->attacker->direction;
    glUniform1f(pe->direction_unif, dir);
    glUniform2f(pe->init_vel_unif, 1, 0);
    glUniformMatrix3fv(program->camera, 1, GL_FALSE, camera.d);
    glUniformMatrix3fv(program->transform, 1, GL_FALSE, pe->position.d);
    glUniform1f(program->time, (float)elapsed / 1000000.f);
    
    glBindVertexArray(box.vertexArrayObject);
    glDrawArraysInstanced(GL_TRIANGLES, 0, box.numVertecies, 100);
    
    return elapsed < 500000ll;
}

BINDABLE(draw_parry_effect, parry_effect_t)

bound_t make_parry_effect(stickman_t* sm)
{
    parry_effect_t pe;
    pe.attacker = sm;
    pe.start_time = game_time.current_time;
    pe.color_unif = glGetUniformLocation(sm->parry_effect.program, "main_color");
    pe.direction_unif = glGetUniformLocation(sm->parry_effect.program, "direction");
    pe.init_vel_unif = glGetUniformLocation(sm->parry_effect.program, "init_vel");
    
    pe.position = affine(0., (sm->next.ground_pos + .4)*sm->direction, 1);
    pe.position.d[0] = sm->direction;
    
    return bind_draw_parry_effect(&pe);
}

//State change forcing

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
//Also, 'frame' is the number of the previous frame.
void stickman_actions(stickman_t* sm, int advance, int attack)
{
    sm->prev = sm->next;
    
    switch (sm->prev.state) {
        case idle:
            if (attack)
                goto_state(sm, base_mid);
            break;
        case base_mid:
            next_state(sm, mid_bot);
            break;
        case mid_bot:
            next_state(sm, idle);
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

typedef enum attack_result {
    WHIFFED = 1<<0,
    LANDED = 1<<1,
    PARRIED = 1<<2,
    KNOCKED = 1<<3,
} attack_result_t;

//The attack is resolved against the previous state. This
//is so the character which is updated first doesn't have
//an advantage.
attack_result_t attack(stickman_t* attacker, attack_t attack)
{
    stickman_t* victim = attacker->other;
    
    if (attacker->prev.state != attack.state
        || game_time.frame - attacker->anim_start != attack.frame)
        return 0;
    
    if (-victim->prev.ground_pos - attacker->prev.ground_pos > attack.range)
        return WHIFFED;
    
    attack_result_t result = 0;
    strike_point_t* target = &victim->prev.fight_state.hi + attack.target;
    
    int effective_momentum = attack.momentum - target->momentum;
    if (effective_momentum >= attack.momentum / 2) //Attack lands
    {
        int damage = attack.damage - target->defense;
        victim->next.health -= damage;
        if (victim->next.health < 0)
            victim->next.health = 0;
        
        result |= LANDED;
    }
    else
        result |= PARRIED;
    
    int knockback = effective_momentum - victim->prev.fight_state.balance;
    if (knockback > 0)
    {
        victim->next.advancing = 0;
        goto_state(victim, idle);
        victim->next.ground_pos -= .02*knockback;
        
        result |= KNOCKED;
    }
    
    return result;
}

#define T(target) (&((fight_state_t*)NULL)->target - &((fight_state_t*)NULL)->hi)

void stickman_consequences(stickman_t* sm)
{
    //              state, frame, target, range, damage, momentum
    switch (attack(sm, (attack_t){base_mid, 1, T(hi), .6, 20, 20}))
    {
        case LANDED:
            push_effect(&effects, make_hit_effect(sm));
            break;
        case PARRIED:
            push_effect(&effects, make_parry_effect(sm));
            break;
        default:
            ;
    }
    
}

//'frame' is also the number of the previous frame here.
void draw_stickman(stickman_t* sm)
{
    glUseProgram(sm->program.program);
    glBindVertexArray(sm->obj.vertexArrayObject);
    
    glUniformMatrix3fv(sm->program.camera, 1, GL_FALSE, camera.d);
    
    float ground_pos = sm->prev.ground_pos * (1. - game_time.alpha)
                     + sm->next.ground_pos * game_time.alpha;
    
    Mat3 pos = affine(0., ground_pos * sm->direction, 0.);
    pos.d[0] = sm->direction;
    glUniformMatrix3fv(sm->program.transform, 1, GL_FALSE, pos.d);
    
    run_anim(sm);
    
    glDrawArrays(GL_TRIANGLES, 0, sm->obj.numVertecies);
    
    draw_health_bar(&sm->health_bar, sm->prev.health);
}


void make_stickman(stickman_t *sm, stickman_t* other, direction_t direction)
{
    make_heath_bar(&sm->health_bar, direction);
    load_shader_program(&sm->hit_effect, screenspace_vert, blast_frag);
    load_shader_program(&sm->parry_effect, particles_vert, color_frag);
    make_anim_obj(&sm->obj, stickman_verts, sizeof(stickman_verts), stickman_stride);
    load_shader_program(&sm->program, anim_vert, color_frag);
    sm->color_unif = glGetUniformLocation(sm->program.program, "main_color");
    glUniform3f(sm->color_unif, 1., 1., 1.);
    
    sm->prev = sm->next = (stickman_state_t){
        .advancing = 0,
        .ground_pos = -1.f,
        .health = 100,
    };
    
    sm->direction = direction;
    sm->other = other;
    
    goto_state(sm, idle);
}

void free_stickman(stickman_t* sm)
{
    free_health_bar(&sm->health_bar);
    free_program(&sm->hit_effect);
    free_program(&sm->parry_effect);
    free_object(&sm->obj);
    free_program(&sm->program);
}
