//
//  character.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/20/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef character_h
#define character_h
#include "types.h"

typedef enum direction {
    LEFT = -1,
    RIGHT = 1
} direction_t;

typedef struct attack {
    int frame;
    ptrdiff_t target;
    float range;
    int damage, momentum;
} attack_t;

typedef enum attack_result {
    WHIFFED = 1<<0,
    LANDED = 1<<1,
    PARRIED = 1<<2,
    KNOCKED = 1<<3,
} attack_result_t;

typedef struct strike_point {
    int defense, momentum;
} strike_point_t;

typedef struct fight_state {
    strike_point_t hi, mid, lo;
    int balance;
} fight_state_t;

#define T(target) (&((fight_state_t*)NULL)->target - &((fight_state_t*)NULL)->hi)

typedef struct state
{
    int frames;
    const anim_step_t* anim;
    fight_state_t fight_state;
} state_t;

typedef struct health_bar {
    object_t obj;
    program_t program;
    GLint health_unif, last_health_unif, time_since_change_unif;
    int health, last_health;
    usec_t last_health_change_time;
} health_bar_t;

typedef struct character_state {
    int state;
    float ground_pos;
    fight_state_t fight_state;
    attack_result_t attack_result;
    int health;
} character_state_t;

typedef struct character {
    object_t obj;
    program_t program;
    direction_t direction;
    const state_t* states;
    struct character* other;
    health_bar_t health_bar;
    
    int attack_button, advance_button;
    long long anim_start;
    character_state_t prev, next;
} character_t;

void goto_state(character_t *c, int state);
void next_state(character_t *c, int state);
void run_anim(character_t *c);
void step_character(character_t* c, int advance_button, int attack_button);
void attack(character_t* attacker, attack_t* attack);
void draw_character(character_t* c);

void make_heath_bar(health_bar_t* hb, direction_t direction);
void draw_health_bar(character_t *c);
void free_health_bar(health_bar_t* hb);

#endif /* character_h */
