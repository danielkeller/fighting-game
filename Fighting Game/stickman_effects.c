//
//  stickman_effects.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/20/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "stickman.h"
#include "engine.h"

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
    
    return elapsed < 150000ll;
}

BINDABLE(draw_hit_effect, hit_effect_t)

bound_t make_hit_effect(stickman_t* sm)
{
    hit_effect_t he;
    he.attacker = sm;
    he.start_time = game_time.current_time;
    he.origin_unif = glGetUniformLocation(sm->hit_effect.program, "origin");
    he.x = (sm->character.next.ground_pos + 2*stickman_hitbox_width)*sm->character.direction;
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

bound_t make_parry_effect(stickman_t* sm, float y)
{
    parry_effect_t pe;
    pe.attacker = sm;
    pe.start_time = game_time.current_time;
    pe.color_unif = glGetUniformLocation(sm->parry_effect.program, "main_color");
    pe.direction_unif = glGetUniformLocation(sm->parry_effect.program, "direction");
    pe.init_vel_unif = glGetUniformLocation(sm->parry_effect.program, "init_vel");
    
    pe.position = affine(0., (sm->character.next.ground_pos + stickman_hitbox_width)*sm->character.direction, y);
    pe.position.d[0] = sm->character.direction;
    
    return bind_draw_parry_effect(&pe);
}
