//
//  stickman.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/1/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "character_internal.h"
#include "objects/stickman.h"
#include "engine.h"

static const float speed = .015;
static const float dodge = .08;
static const float block_dist = .3;

//For attacks that give defense/momentum buffs, used symmetrically:
//If the attack is before the midpoint of the animation, advantage goes to the
//first player. After the midpoint, it goes to the second. If it is at the midpoint
//either both attacks land or neither.

//            frames  next  fwd_speed rev_speed balance   hi        lo
static const state_t states[] = {
    [top]       = {1, top,       speed, speed, {10, {8,  MIDDLE}, {1,  WEAK}}},
    [bottom]    = {1, bottom,    speed, speed, {8,  {0,  WEAK},   {8,  MIDDLE}}},
    [swing_1]   = {4, swing_2,   .025,  0,     {6,  {10, HEAVY},  {1,  WEAK}}},
    [swing_2]   = {3, bottom,    .025,  0,     {6,  {10, HEAVY},  {1,  WEAK}}},
    [swingup_1] = {5, swingup_2, .02,   0,     {12, {0,  WEAK},   {10, HEAVY}}},
    [swingup_2] = {4, top,       .02,   0,     {12, {0,  WEAK},   {10, HEAVY}}},
    
    [hi_block]   = {2, block,      0, block_dist/2., {5, {1, WEAK}, {1, WEAK}}},
    [lo_block_1] = {1, lo_block_2, 0, block_dist/3., {5, {1, WEAK}, {1, WEAK}}},
    [lo_block_2] = {1, lo_block_3, 0, block_dist/3., {5, {1, WEAK}, {1, WEAK}}},
    [lo_block_3] = {1, block,      0, block_dist/3., {5, {1, WEAK}, {1, WEAK}}},
    [block]      = {6, unblock,    0, 0, {10, {20, HEAVY}, {20, HEAVY}}},
    [unblock]    = {3, top,        0, 0, {10, {1, WEAK}, {1, WEAK}}},
};

static attack_t
down_attack = {2, T(hi), .6, 20, 20, MIDDLE},
up_attack   = {1, T(lo), .6, 10, 30, MIDDLE};

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
//So the states and transitions for a 2-frame state look like this:
//       _____#-----=-----=.....@
// anim_start ^  +1 ^  +2 ^
// where '=' are calls to _actions() in the '-' state
int stickman_actions(stickman_t* sm)
{
    character_t* c = sm->character;
    
    move_character(c);
    
    int can_dodge = DODGE && (!SCOOCH || c->prev.ground_pos > -1. + block_dist || c->prev.advancing);
    
    switch (c->prev.state) {
        case top:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, swing_1);
            if (SHIFT_FLAG(c->dodge_button) && can_dodge)
                goto_state(c, hi_block);
            break;
        case swing_1:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, top);
            if (SHIFT_FLAG(c->dodge_button) && can_dodge)
                goto_state(c, hi_block);
            break;
        case swing_2:
            attack(c, &down_attack);
            if (c->next.attack_result & LANDED)  push_effect(&effects, make_hit_effect(sm));
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, 1.));
            break;
            
        case bottom:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, swingup_1);
            if (SHIFT_FLAG(c->dodge_button) && can_dodge)
                goto_state(c, lo_block_1);
            break;
        case swingup_1:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, bottom);
            if (SHIFT_FLAG(c->dodge_button) && can_dodge)
                goto_state(c, lo_block_1);
            break;
        case swingup_2:
            attack(c, &up_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .5));
            break;
            
        default:
            break;
    }
    
    next_state(c);
    anim_obj_keys(&sm->obj, &stickman_anims[c->next.state]);
    
    //This effect is annoying
    //if (c->other->prev.attack_result & KNOCKED)
    //    goto_state(c, bottom);
    
    return 0;
}
BINDABLE(stickman_actions, stickman_t)

//'frame' is also the number of the previous frame here.
int draw_stickman(stickman_t* sm)
{
    glUseProgram(sm->program.program);
    set_character_uniforms(sm->character, &sm->program);
    glBindVertexArray(sm->obj.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, sm->obj.numVertecies);
    
    draw_health_bar(sm->character);
    return 0;
}
BINDABLE(draw_stickman, stickman_t)

int free_stickman(stickman_t* sm)
{
    character_t* c = sm->character;
    
    free_health_bar(&c->health_bar);
    free_program(&sm->hit_effect);
    free_program(&sm->parry_effect);
    free_object(&sm->obj);
    free_program(&sm->program);
    
    free(sm);
    return 0;
}
BINDABLE(free_stickman, stickman_t)

void make_stickman(character_t* c, character_t* other, direction_t direction)
{
    stickman_t* sm = malloc(sizeof(stickman_t));
    
    sm->character = c;
    sm->character->other = other;
    
    c->actions = ref_bind_stickman_actions(sm);
    c->draw = ref_bind_draw_stickman(sm);
    c->free = ref_bind_free_stickman(sm);
    
    make_heath_bar(&sm->character->health_bar, direction);
    load_shader_program(&sm->hit_effect, screenspace_vert, blast_frag);
    load_shader_program(&sm->parry_effect, particles_vert, color_frag);
    
    make_anim_obj(&sm->obj, stickman_verts, sizeof(stickman_verts), stickman_stride);
    load_shader_program(&sm->program, anim_vert, color_frag);
    sm->color_unif = glGetUniformLocation(sm->program.program, "main_color");
    glUniform3f(sm->color_unif, 1., 1., 1.);
    
    c->states = states;
    c->speed = speed;
    c->dodge = dodge;
    c->hitbox_width = stickman_hitbox_width;
    c->prev = c->next = (character_state_t){
        .ground_pos = -1.f,
        .health = 100,
        .advancing = 0,
    };
    
    c->direction = direction;
    c->dodge_button = c->attack_button = 0;
    
    goto_state(c, top);
}
