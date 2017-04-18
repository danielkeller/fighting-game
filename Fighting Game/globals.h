//
//  world.h
//  Fighting Game
//
//  Created by Daniel Keller on 9/3/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef globals_h
#define globals_h
#include "math.h"
#include "object.h"
#include "fbo.h"
#include "time.h"
#include "effects.h"

extern Mat3 camera;
extern fbo_t fbo;
extern game_time_t game_time;
extern object_t box;
extern effects_t effects;

#endif /* world_h */
