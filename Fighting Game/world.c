//
//  world.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/3/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "engine.h"
#include <math.h>

Mat3 camera = {{
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
}};

struct key_events no_key_events; //Auto-initialized to 0
fbo_t fbo;
struct game_time game_time;
struct object box;
effects_t effects;
int learning_mode = 0;

void calculate_camera(float win_width, float win_height)
{
    //aspect is 16:9 or 2:1.125
    float max_height = win_width * 9 / 16,
        max_width = win_height * 16 / 9;
    
    float width = fmin(win_width, max_width),
        height = fmin(win_height, max_height);
    
    //x scale
    camera.d[0] = width / win_width;
    //y scale
    camera.d[4] = height / win_height * 16. / 9.;
    //y shift
    camera.d[7] = (win_height - height) / win_height - 1.;
}
