//
//  stickman.h
//  Fighting Game
//
//  Created by Daniel Keller on 9/1/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef stickman_h
#define stickman_h
#include "character.h"

typedef struct stickman {
    character_t character;
    GLint color_unif;
    program_t hit_effect, parry_effect;
} stickman_t;

void make_stickman(stickman_t*, character_t* other, direction_t direction);
//These might be bindable
void stickman_actions(stickman_t*);
void draw_stickman(stickman_t*);
void free_stickman(stickman_t*);

//Internal
static const float stickman_hitbox_width = .25;

typedef struct bound bound_t;
bound_t make_hit_effect(stickman_t* sm);
bound_t make_parry_effect(stickman_t* sm, float y);

#endif /* stickman_h */
