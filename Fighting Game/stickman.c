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

//For attacks that give defense/momentum buffs, used symmetrically:
//If the attack is before the midpoint of the animation, advantage goes to the
//first player. After the midpoint, it goes to the second. If it is at the midpoint
//either both attacks land or neither.

//         frames  balance   hi        lo
static const state_t states[] = {
    [top]       = {1, {10, {8,  MIDDLE}, {1,  WEAK}}},
    [bottom]    = {1, {8,  {0,  WEAK},   {8,  MIDDLE}}},
    [swing_1]   = {2, {6,  {10, HEAVY},  {1,  WEAK}}},
    [swing_2]   = {3, {6,  {10, HEAVY},  {1,  WEAK}}},
    [swingup_1] = {4, {12, {0,  WEAK},   {10, HEAVY}}},
    [swingup_2] = {3, {12, {0,  WEAK},   {10, HEAVY}}},
};

static attack_t
down_attack = {2, T(hi), .6, 20, 20, MIDDLE},
up_attack   = {3, T(lo), .6, 10, 30, MIDDLE};

static const float speed = .02;
static const float dodge = .08;

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
//So the states and transitions for a 2-frame state look like this:
//       _____#-----=-----=.....@
// anim_start ^  +1 ^  +2 ^
// where '=' are calls to _actions() in the '-' state
void stickman_actions(stickman_t* sm)
{
    character_t* c = &sm->character;
    
    switch (c->prev.state) {
        case top:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, swing_1);
            break;
        case swing_1:
            next_state(c, swing_2);
            break;
        case swing_2:
            attack(c, &down_attack);
            if (c->next.attack_result & LANDED)  push_effect(&effects, make_hit_effect(sm));
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, 1.));
            next_state(c, bottom);
            break;
            
        case bottom:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, swingup_1);
            break;
        case swingup_1:
            attack(c, &up_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .5));
            next_state(c, swingup_2);
            break;
        case swingup_2:
            next_state(c, top);
            break;
        default:
            break;
    }
    
    //This effect is annoying
    //if (c->other->prev.attack_result & KNOCKED)
    //    goto_state(c, bottom);
    
    //Another possible form of movement is the character always moves forwards,
    //or moves forwards when attacking, and there is a dodge button. This is
    //more like a "horizontal position as recharging dodge bar."
    float other_pos = -c->other->next.ground_pos;
    float fwd_limit = fmin(1, other_pos) - stickman_hitbox_width*2.;
    float move_dir = c->advance_button ? 1. : -1.;
    float move_speed = c->prev.advancing == c->advance_button ? speed : dodge;
    c->next.ground_pos += move_dir*move_speed;
    c->next.ground_pos = fmin(fmax(c->next.ground_pos, -1), fwd_limit);
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
    c->anims = stickman_anims;
    c->prev = c->next = (character_state_t){
        .ground_pos = -1.f,
        .health = 100,
        .advancing = 0,
    };
    
    c->direction = direction;
    c->other = other;
    c->advance_button = c->attack_button = 0;
    
    goto_state(c, top);
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
