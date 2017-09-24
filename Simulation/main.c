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
#include <stdint.h>
#include <string.h>

const unsigned max_input = (5 << 4) + 10;

struct key_events int_to_input(unsigned i)
{
    unsigned d = i >> 4; //down
    unsigned e = i & 0xF; //event
    return (struct key_events){
        {d==1, e==1, e==2},
        {d==2, e==3, e==4},
        {d==3, e==5, e==6},
        {d==4, e==7, e==8},
        {d==5, e==9, e==10},
    };
}

void explore_state_space(character_t* c, int num_states)
{
    for (int state = 0; state < num_states; ++state) {
        int gotten_times[num_states]; memset(gotten_times, 0, sizeof gotten_times);
        unsigned gotten_input[num_states]; memset(gotten_input, 0, sizeof gotten_input);
        frame_t gotten_frame[num_states]; memset(gotten_frame, 0, sizeof gotten_frame);
        
        for (frame_t frame = 0; frame <= c->states[state].frames; ++frame) {
            game_time.frame = frame;
            
            //Used to pick the simplest input
            int gotten_states[num_states]; memset(gotten_states, 0, sizeof gotten_states);
            for (unsigned input = 0; input < max_input; ++input) {
                c->anim_start = 0;
                c->next.state = state;
                struct key_events key_events = int_to_input(input);
                
                step_character(c, &key_events);
                character_actions(c);
                int found_state = c->next.state;
                
                if (!gotten_states[found_state]) {
                    gotten_states[found_state] = 1;
                    ++gotten_times[found_state];
                    gotten_input[found_state] = input;
                    gotten_frame[found_state] = frame;
                }
            }
        }
        
        printf("%-2d ", state);
        
        //print missing self-loop
        if (gotten_times[state] > 1 && gotten_input[state] == 0)
            printf(" ");
        else
            printf("X");
        
        //print default exit
        int exit = c->states[state].next_state;
        if (gotten_times[exit] == 1 && gotten_input[exit] == 0)
            printf(">%-2d ", exit);
        else
            printf("-   ");
        
        for (int found_state = 0; found_state < num_states; ++found_state) {
            if (found_state == state && gotten_times[state] > 1 && gotten_input[state] == 0)
                continue; //self-loop
            if (found_state == exit && gotten_times[exit] == 1 && gotten_input[exit] == 0)
                continue; //default exit
            
            if (gotten_times[found_state] > 0) {
                printf("->%-2d %2d ", found_state, gotten_input[found_state]);
                if (gotten_times[found_state] > 1)
                    printf(" *  ");
                else
                    printf("%2llu  ", gotten_frame[found_state]);
            }
        }
        printf("\n");
    }
}

#define NUM_STATES 25

int main (int argc, char* argv[])
{
    struct character stickman, fatman;
    struct stickman struct_stickman;
    struct fatman struct_fatman;
    
    game_time.frame = 0;
    make_simulation_stickman(&struct_stickman, &stickman, &fatman);
    make_simulation_fatman(&struct_fatman, &fatman, &stickman);
    
    explore_state_space(&stickman, 25);
    explore_state_space(&fatman, 19);
    
    return 0;
}
