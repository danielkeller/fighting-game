//
//  effects.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/18/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef effects_h
#define effects_h
#include "function.h"

#define MAX_EFFECTS 128

typedef struct effects {
    int num_effects;
    bound_t effects[MAX_EFFECTS];
} effects_t;

void make_effects(effects_t*);

//Takes ownership of effect
void push_effect(effects_t*, bound_t effect);

void draw_effects(effects_t*);
void free_effects(effects_t*);

#endif /* effects_h */
