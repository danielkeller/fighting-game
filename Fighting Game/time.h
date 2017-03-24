//
//  time.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/5/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef time_h
#define time_h

typedef long long usec_t;

//25 FPS
static const usec_t tick_length = 40000ll;
//4 FPS
static const usec_t frame_limit = 250000ll;

typedef struct game_time {
    unsigned long long frame;
    usec_t last_render, last_frame_length, current_time, unsimulated_time;
} game_time_t;

void init_game_time(game_time_t*);

usec_t get_time();

//Update the game_time for one physics tick, and return 1 if we should simulate
int phys_tick(game_time_t*);
//Update the game_time for one render tick, and return the leftover time as a fraction
//of the tick length; [0,1).
//Note that the just-simulated tick is considered to be in the past, and the rendering
//should be extrapolated from it.
float render_tick(game_time_t*);

#endif /* time_h */
