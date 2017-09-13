//
//  input.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/13/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "engine.h"

//Replace the normal input handling with one that doesn't disturb the source

int shift_button_press(struct button* b)
{
    return b->pressed != 0;
}

int shift_button_cancel(struct button* b)
{
    return b->cancelled != 0;
}

void update_button(struct button* to, struct button* from)
{
    to->down = from->down;
    to->pressed = from->pressed;
    to->cancelled = from->cancelled;
}

void update_key_events(struct key_events* to, struct key_events* from)
{
    update_button(&to->attack, &from->attack);
    update_button(&to->dodge, &from->dodge);
    update_button(&to->fwd, &from->fwd);
    update_button(&to->back, &from->back);
    update_button(&to->special, &from->special);
}
