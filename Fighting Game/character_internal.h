//
//  character_internal.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/27/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef character_internal_h
#define character_internal_h
#include "character.h"

#define T(target) (&((fight_state_t*)NULL)->target - &((fight_state_t*)NULL)->hi)

typedef struct attack {
    //The frame of the attack can never be 0. This is intentional: at frame 0 of a
    //state, only next.state equals that state, and attacks are resolved against prev.
    //To put an attack on a state transition, put it at the end of the previous state.
    int frame;
    ptrdiff_t target;
    float range;
    int damage, knock;
    force_t force;
} attack_t;

void goto_state(character_t *c, int state);
void next_state(character_t *c);

void move_character(character_t* c);
void attack(character_t* attacker, attack_t* attack);

void set_character_draw_state(character_t* c, program_t* program);

void make_heath_bar(health_bar_t* hb, direction_t direction);
void draw_health_bar(character_t *c);
void free_health_bar(health_bar_t* hb);

void make_state_indicator(state_indicator_t*);
void draw_state_indicator(character_t*);
void free_state_indicator(state_indicator_t*);

//*** Stickman
typedef struct stickman {
    character_t* character;
    program_t program;
    object_t obj;
    GLint color_unif;
    program_t hit_effect, parry_effect;
} stickman_t;

static const float stickman_hitbox_width = .25;

typedef struct bound bound_t;
bound_t make_hit_effect(stickman_t* sm);
bound_t make_parry_effect(stickman_t* sm, float y);

#endif /* character_internal_h */
