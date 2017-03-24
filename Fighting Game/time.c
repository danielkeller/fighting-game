//
//  time.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/5/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "time.h"

#include "window.h"
//todo: utils.h for die
#include "error.h"

usec_t get_time()
{
    //Overflows after 214 days
    uint64_t time_mCount = glfwGetTimerValue() * 1000;
    uint64_t freq_kHz = glfwGetTimerFrequency() / 1000;
    return time_mCount / freq_kHz;
}

void init_game_time(game_time_t* gt)
{
    gt->frame = 0;
    gt->current_time = gt->unsimulated_time = 0;
    gt->last_frame_length = 0;
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
    gt->last_frame_length = frame_time;
    
    //The amount of unsimulated time after the next physics step(s)
    usec_t next_ut = gt->unsimulated_time % tick_length;
    return (float)next_ut / (float)tick_length;
}
