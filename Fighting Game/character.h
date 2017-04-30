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
#include "function.h"
#include "options.h"

typedef struct character character_t;

typedef enum direction {
    LEFT = -1,
    RIGHT = 1
} direction_t;

int step_character(character_t*, int move_button, int dodge_button, int attack_button);
void character_actions(character_t*);
void draw_character(character_t*);
//Must clear effects after calling this
void free_character(character_t*);

//Relies on init_game_time having been called
void make_stickman(character_t* c, character_t* other, direction_t direction);

typedef enum force {
    WEAK, LIGHT, MIDDLE, HEAVY, XHEAVY
} force_t;

typedef enum attack_result {
    WHIFFED = 1<<0,
    LANDED  = 1<<1,
    PARRIED = 1<<2,
    KNOCKED = 1<<3,
} attack_result_t;

typedef struct strike_point {
    int defense;
    force_t block;
} strike_point_t;

typedef struct fight_state {
    int balance;
    strike_point_t hi, lo;
} fight_state_t;

typedef struct state
{
    int frames;
    int next_state;
    float fwd_speed, rev_speed;
    fight_state_t fight_state;
} state_t;

typedef struct health_bar {
    program_t program;
    GLint health_unif, last_health_unif, time_since_change_unif;
    int health, last_health;
    usec_t last_health_change_time;
} health_bar_t;

typedef struct state_indicator {
    program_t program;
    GLint top_unif, bot_unif, top_attack_unif, bot_attack_unif;
    force_t top_attack, bot_attack;
    usec_t last_attack_time;
} state_indicator_t;

typedef struct character_state {
    int state;
    float ground_pos;
    int advancing;
    fight_state_t fight_state;
    attack_result_t attack_result;
    int health;
} character_state_t;

typedef struct character {
    bound_t actions, draw, free;
    
    const state_t* states;
    float hitbox_width;
    float speed, dodge;
    
    direction_t direction;
    struct character* other;
    health_bar_t health_bar;
    state_indicator_t state_indicator;
    
    int move_button, attack_button, dodge_button;
    long long anim_start;
    character_state_t prev, next;
    float ground_pos;
} character_t;

#endif /* character_h */
