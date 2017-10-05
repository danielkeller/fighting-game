//
//  screen_effects.h
//  Fighting Game
//
//  Created by Daniel Keller on 10/4/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef screen_effects_h
#define screen_effects_h
#include "engine.h"

struct screen_effects {
    struct program screen_noise, blur, waves;
    GLint blur_intensity_unif, waves_intensity_unif;
    fbo_t noise_fbo;
};

void make_screen_effects(struct screen_effects*);
void draw_screen_effects(struct screen_effects*, float intensity);
void free_screen_effects(struct screen_effects*);

#endif /* screen_effects_h */
