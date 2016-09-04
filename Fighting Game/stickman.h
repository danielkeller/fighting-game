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

typedef struct stickman {
    object_t obj;
    program_t program;
    GLint color_unif;
    int state;
    long long anim_start;
} stickman_t;

void make_stickman(stickman_t*);
void update_stickman(stickman_t*, long long frame, int attack);
void draw_stickman(stickman_t*, long long frame, float alpha);
void free_stickman(stickman_t*);

#endif /* stickman_h */
