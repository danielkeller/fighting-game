//
//  time.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/5/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "engine.h"
#include "window.h"
#include "error.h"

usec_t get_time()
{
    //Overflows after 214 days
    uint64_t time_mCount = glfwGetTimerValue() * 1000;
    uint64_t freq_kHz = glfwGetTimerFrequency() / 1000;
    return time_mCount / freq_kHz;
}

void init_game_time(struct game_time* gt)
{
    gt->frame = 0;
    gt->current_time = gt->unsimulated_time = 0;
    gt->last_frame_length = 0;
    gt->multiplier = 1;
    gt->last_render = get_time();
}

//The "Fix Your Timestep" algorithm

int phys_tick(struct game_time *gt)
{
    if (gt->unsimulated_time < tick_length)
        return 0;
    
    ++gt->frame;
    gt->unsimulated_time -= tick_length;
    return 1;
}

void render_tick(struct game_time *gt)
{
    usec_t now = get_time();
    usec_t frame_time = (now - gt->last_render) / gt->multiplier;
    if (frame_time > frame_limit) frame_time = frame_limit;
    
    gt->last_render = now;
    gt->unsimulated_time += frame_time;
    gt->current_time += frame_time;
    gt->last_frame_length = frame_time;
    
    //The amount of unsimulated time after the next physics step(s)
    usec_t next_ut = gt->unsimulated_time % tick_length;
    gt->alpha = (float)next_ut / (float)tick_length;
}
