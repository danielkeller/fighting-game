//
//  world.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/3/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "globals.h"

Mat3 camera = {{
    1, 0, 0,
    0, 1, 0,
    0, -.5, 1
}};

fbo_t fbo;
game_time_t game_time;
object_t box;
effects_t effects;