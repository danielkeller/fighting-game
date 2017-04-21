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
    top, top_mid, mid_bot,
    bottom, bot_mid, mid_top,
};

static const state_t states[] = {
    [top] = {1, &stickman_top_Basis_Basis,
        {.balance = 10, .hi = {8, 10}, .mid = {2, 3}}},
    [bottom] = {1, &stickman_bottom_DS_Bot_DS_Bot,
        {.balance = 8, .hi = {0, 1}, .mid = {8, 10}}},
    [top_mid] = {2, &stickman_swing_Basis_DS_Mid,
        {.balance = 6, .hi = {10, 16}, .mid = {1, 1}}},
    [mid_bot] = {3, &stickman_swing_DS_Mid_DS_Bot,
        {.balance = 6, .hi = {10, 16}, .mid = {1, 1}}},
    [bot_mid] = {4, &stickman_swingup_DS_Bot_DS_Mid,
        {.balance = 12, .hi = {0, 1}, .mid = {10, 16}}},
    [mid_top] = {3, &stickman_swingup_DS_Mid_Basis,
        {.balance = 12, .hi = {0, 1}, .mid = {10, 16}}},
};

attack_t
down_attack = {1, T(hi), .6, 20, 20},
up_attack = {3, T(mid), .6, 10, 30};

static const float speed = .03;

//Note that new actions are considered to start epsilon after the previous frame,
//as opposed to on the start of the next frame. (the previous frame is stored in anim_start)
void stickman_actions(stickman_t* sm)
{
    character_t* c = &sm->character;
    
    switch (c->prev.state) {
        case top:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, top_mid);
            break;
        case top_mid:
            next_state(c, mid_bot);
            attack(c, &down_attack);
            if (c->next.attack_result & LANDED)  push_effect(&effects, make_hit_effect(sm));
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, 1.));
            break;
        case mid_bot:
            next_state(c, bottom);
            break;
            
        case bottom:
            if (SHIFT_FLAG(c->attack_button))
                goto_state(c, bot_mid);
            break;
        case bot_mid:
            next_state(c, mid_top);
            attack(c, &up_attack);
            if (c->next.attack_result & PARRIED) push_effect(&effects, make_parry_effect(sm, .5));
            break;
        case mid_top:
            next_state(c, top);
            break;
        default:
            break;
    }
    
    //This effect is annoying
    //if (c->other->prev.attack_result & KNOCKED)
    //    goto_state(c, bottom);
    
    float other_pos = -c->other->next.ground_pos;
    float fwd_limit = fmin(1, other_pos) - stickman_hitbox_width*2.;
    
    if (c->advance_button)
        c->next.ground_pos = fmin(fwd_limit, c->next.ground_pos + speed);
    else
        c->next.ground_pos = fmax(-1, c->next.ground_pos - speed);
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
        .ground_pos = -1.f,
        .health = 100,
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
