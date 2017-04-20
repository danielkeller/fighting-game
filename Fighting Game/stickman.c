//
//  stickman.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/1/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "stickman.h"
#include "objects/stickman.h"
#include "engine.h"
#include <math.h>
#include <stdlib.h>

enum state_names {
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
    he.x = (sm->character.next.ground_pos + .4)*sm->character.direction;
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
    float dir = (float)pe->attacker->character.direction;
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
    
    pe.position = affine(0., (sm->character.next.ground_pos + .4)*sm->character.direction, 1);
    pe.position.d[0] = sm->character.direction;
    
    return bind_draw_parry_effect(&pe);
}

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
//Also, 'frame' is the number of the previous frame.
void stickman_actions(stickman_t* sm, int advance, int attack)
{
    character_t* c = &sm->character;
    c->prev = c->next;
    
    switch (c->prev.state) {
        case idle:
            if (attack)
                goto_state(c, base_mid);
            break;
        case base_mid:
            next_state(c, mid_bot);
            break;
        case mid_bot:
            next_state(c, idle);
            break;
        default:
            break;
    }
    
    float other_pos = -c->other->next.ground_pos;
    float fwd_limit = fmin(1, other_pos) - hitbox_width*2.;
    
    if (advance && c->prev.ground_pos != fwd_limit)
        c->next.ground_pos = fmin(fwd_limit, c->prev.ground_pos + speed);
    else if (!advance && c->prev.ground_pos != -1.)
        c->next.ground_pos = fmax(-1, c->prev.ground_pos - speed);

    c->next.advancing = advance;
}

void stickman_consequences(stickman_t* sm)
{
    character_t* c = &sm->character;
    
    //              state, frame, target, range, damage, momentum
    attack_result_t res = attack(c, (attack_t){base_mid, 1, T(hi), .6, 20, 20});
    if (res & LANDED)  push_effect(&effects, make_hit_effect(sm));
    if (res & PARRIED) push_effect(&effects, make_parry_effect(sm));
    //???
    if (res & KNOCKED) goto_state(sm->character.other, idle);
}

//'frame' is also the number of the previous frame here.
void draw_stickman(stickman_t* sm)
{
    draw_character(&sm->character);
    draw_health_bar(&sm->character);
}


void make_stickman(stickman_t *sm, character_t* other, direction_t direction)
{
    character_t* c = &sm->character;
    
    make_heath_bar(&sm->character.health_bar, direction);
    load_shader_program(&sm->hit_effect, screenspace_vert, blast_frag);
    load_shader_program(&sm->parry_effect, particles_vert, color_frag);
    
    make_anim_obj(&sm->character.obj, stickman_verts, sizeof(stickman_verts), stickman_stride);
    load_shader_program(&sm->character.program, anim_vert, color_frag);
    sm->color_unif = glGetUniformLocation(sm->character.program.program, "main_color");
    glUniform3f(sm->color_unif, 1., 1., 1.);
    
    c->states = states;
    c->prev = c->next = (character_state_t){
        .advancing = 0,
        .ground_pos = -1.f,
        .health = 100,
    };
    
    c->direction = direction;
    c->other = other;
    
    goto_state(c, idle);
}

void free_stickman(stickman_t* sm)
{
    character_t* c = &sm->character;
    
    free_health_bar(&c->health_bar);
    free_program(&sm->hit_effect);
    free_program(&sm->parry_effect);
    free_object(&c->obj);
    free_program(&c->program);
}
