//
//  stickman.h
//  Fighting Game
//
//  Created by Daniel Keller on 9/1/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef stickman_h
#define stickman_h

#include "object.h"
#include "shader.h"
typedef struct fbo fbo_t;
typedef struct game_time game_time_t;

typedef enum direction {
    LEFT = -1,
    RIGHT = 1
} direction_t;

typedef struct attack {
    int state, frame;
    ptrdiff_t target;
    float range;
    int damage, momentum;
} attack_t;

typedef struct strike_point {
    int defense, momentum;
} strike_point_t;

typedef struct fight_state {
    strike_point_t hi, mid, lo;
    int balance;
} fight_state_t;

typedef struct state
{
    int frames;
    const anim_step_t* anim;
    fight_state_t fight_state;
} state_t;

typedef struct health_bar {
    object_t obj;
    program_t program;
    GLint health_unif;
} health_bar_t;

typedef struct stickman_state {
    int state;
    int advancing;
    float ground_pos;
    fight_state_t fight_state;
    int health;
} stickman_state_t;

typedef struct stickman {
    object_t obj;
    program_t program;
    GLint color_unif;
    direction_t direction;
    struct stickman* other;
    health_bar_t health_bar;
    program_t hit_effect, parry_effect;
    
    long long anim_start;
    stickman_state_t prev, next;
} stickman_t;

void make_stickman(stickman_t*, stickman_t* other, direction_t direction);
void stickman_actions(stickman_t*, int advance, int attack);
void stickman_consequences(stickman_t*);
void draw_stickman(stickman_t*);
void free_stickman(stickman_t*);

#endif /* stickman_h */
