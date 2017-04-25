//
//  effects.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/18/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "engine.h"

void make_effects(effects_t* ef)
{
    ef->num_effects = 0;
}

//Takes ownership of effect
void push_effect(effects_t* ef, bound_t effect)
{
    ef->effects[ef->num_effects] = effect;
    ++ef->num_effects;
}

void draw_effects(effects_t* ef)
{
    //Also sweep out effects that are done
    int j = 0;
    for (int i = 0; i < ef->num_effects; ++i) {
        if (CALL_BOUND(ef->effects[i])) { //not done
            if (j != i)
                ef->effects[j] = ef->effects[i];
            ++j;
        }
        else
            free_bound(&ef->effects[i]);
    }
    ef->num_effects = j;
}

void clear_effects(effects_t* ef)
{
    for (int i = 0; i < ef->num_effects; ++i)
        free_bound(&ef->effects[i]);
    ef->num_effects = 0;
}

void free_effects(effects_t* ef)
{
    for (int i = 0; i < ef->num_effects; ++i)
        free_bound(&ef->effects[i]);
}
