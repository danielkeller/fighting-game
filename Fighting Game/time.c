//
//  time.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/5/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "time.h"

//todo: utils.h for die
#include "error.h"

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>

usec_t get_time()
{
    //http://stackoverflow.com/a/11681069/603688
    static clock_serv_t cclock;
    mach_timespec_t mts;
    
    static int run = 1;
    for (; run; run = 0)
        if (host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock))
            die("host_get_clock_service");
    
    if (clock_get_time(cclock, &mts))
        die("clock_get_time");
    //if (mach_port_deallocate(mach_task_self(), cclock))
    //    die("mach_port_deallocate");
    
    return (usec_t)mts.tv_sec * 1000000ll + (usec_t)mts.tv_nsec / 1000ll;
}
#else
//...
#endif

void init_game_time(game_time_t* gt)
{
    gt->frame = 0;
    gt->current_time = gt->unsimulated_time = 0;
    gt->last_render = get_time();
}

//The "Fix Your Timestep" algorithm

int phys_tick(game_time_t *gt)
{
    if (gt->unsimulated_time < tick_length)
        return 0;
    
    ++gt->frame;
    gt->unsimulated_time -= tick_length;
    return 1;
}

float render_tick(game_time_t *gt)
{
    usec_t now = get_time();
    usec_t frame_time = now - gt->last_render;
    if (frame_time > frame_limit) frame_time = frame_limit;
    
    gt->last_render = now;
    gt->unsimulated_time += frame_time;
    gt->current_time += frame_time;
    
    //The amount of unsimulated time after the next physics step(s)
    usec_t next_ut = gt->unsimulated_time % tick_length;
    return (float)next_ut / (float)tick_length;
}