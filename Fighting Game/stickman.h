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
#include "time.h"

typedef enum advance {
    ADVANCING,
    RETREATING,
    STATIONARY
} advance_t;

typedef struct stickman_state {
    int state;
    advance_t advancing;
    float ground_pos;
} stickman_state_t;

typedef struct stickman {
    object_t obj;
    program_t program;
    GLint color_unif;
    int direction;
    struct stickman* other;
    
    long long anim_start;
    stickman_state_t prev, next;
} stickman_t;

void make_stickman(stickman_t*, stickman_t* other, int direction);
void update_stickman(stickman_t*, stickman_t* enemy,
                     long long frame, int advance, int attack);
void draw_stickman(stickman_t*, long long frame, float alpha);
void free_stickman(stickman_t*);

#endif /* stickman_h */
