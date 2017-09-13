//
//  main.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/13/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "character_internal.h"
#include "engine.h"
#include <stdio.h>

int main (int argc, char* argv[])
{
    struct character stickman, fatman;
    struct stickman struct_stickman;
    struct fatman struct_fatman;
    
    game_time.frame = 0;
    make_simulation_stickman(&struct_stickman, &stickman, &fatman);
    make_simulation_fatman(&struct_fatman, &fatman, &stickman);
    
    while (game_time.frame < 20) {
        printf("%d ", stickman.next.state);
        step_character(&stickman, &(struct key_events){.attack = {.pressed = 1}});
        stickman_actions(&struct_stickman);
        ++game_time.frame;
    }
    
    printf("\n");
    
    return 0;
}
