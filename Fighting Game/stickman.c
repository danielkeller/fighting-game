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

static const float speed = .01;
static const float block_dist = .3;

//For attacks that give defense/momentum buffs, used symmetrically:
//If the attack is before the midpoint of the animation, advantage goes to the
//first player. After the midpoint, it goes to the second. If it is at the midpoint
//either both attacks land or neither.

//         frames  scooch balance   hi        lo
static const state_t states[] = {
    [top]       = {1, 0,   {10, {8,  MIDDLE}, {1,  WEAK}}},
    [bottom]    = {1, 0,   {8,  {0,  WEAK},   {8,  MIDDLE}}},
    [swing_1]   = {2, .02, {6,  {10, HEAVY},  {1,  WEAK}}},
    [swing_2]   = {3, .02, {6,  {10, HEAVY},  {1,  WEAK}}},
    [swingup_1] = {4, .01, {12, {0,  WEAK},   {10, HEAVY}}},
    [swingup_2] = {3, .01, {12, {0,  WEAK},   {10, HEAVY}}},
    
    [hi_block]   = {2, -block_dist/2., {10, {20, HEAVY}, {8, MIDDLE}}},
    [lo_block_1] = {1, -block_dist/3., {10, {8, MIDDLE}, {20, HEAVY}}},
    [lo_block_2] = {1, -block_dist/3., {10, {8, MIDDLE}, {20, HEAVY}}},
    [lo_block_3] = {1, -block_dist/3., {10, {8, MIDDLE}, {20, HEAVY}}},
    [block]     = {7, -speed, {10, {20, HEAVY}, {20, HEAVY}}},
    [unblock]   = {2, 0, {10, {20, HEAVY}, {8, MIDDLE}}},
};

static attack_t
down_attack = {2, T(hi), .6, 20, 20, MIDDLE},
up_attack   = {3, T(lo), .6, 10, 30, MIDDLE};

#define PASSIVE(from, to) case from: next_state(c, to); break;

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
//So the states and transitions for a 2-frame state look like this:
//       _____#-----=-----=.....@
// anim_start ^  +1 ^  +2 ^
// where '=' are calls to _actions() in the '-' state
void stickman_actions(stickman_t* sm)
{
    character_t* c = &sm->character;
    
    move_character(c);
    
    switch (c->prev.state) {
        case top:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, swing_1);
            if (SHIFT_FLAG(c->dodge_button) && c->prev.ground_pos > -1. + block_dist)
                goto_state(c, hi_block);
            break;
        PASSIVE(swing_1, swing_2)
        case swing_2:
            attack(c, &down_attack);
            if (c->next.attack_result & LANDED)  push_effect(&effects, make_hit_effect(sm));
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, 1.));
            next_state(c, bottom);
            break;
            
        case bottom:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, swingup_1);
            if (SHIFT_FLAG(c->dodge_button) && c->prev.ground_pos > -1. + block_dist)
                goto_state(c, lo_block_1);
            break;
        case swingup_1:
            attack(c, &up_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .5));
            next_state(c, swingup_2);
            break;
        PASSIVE(swingup_2, top)
            
        PASSIVE(hi_block, block)
        PASSIVE(lo_block_1, lo_block_2)
        PASSIVE(lo_block_2, lo_block_3)
        PASSIVE(lo_block_3, block)
        
        PASSIVE(block, unblock)
        PASSIVE(unblock, top)
            
        default:
            break;
    }
    
    //This effect is annoying
    //if (c->other->prev.attack_result & KNOCKED)
    //    goto_state(c, bottom);
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
    c->speed = speed;
    c->hitbox_width = stickman_hitbox_width;
    c->prev = c->next = (character_state_t){
        .ground_pos = -.5f,
        .health = 100,
    };
    
    c->direction = direction;
    c->other = other;
    c->dodge_button = c->attack_button = 0;
    
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
