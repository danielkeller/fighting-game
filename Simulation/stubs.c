//
//  stubs.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/13/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "engine.h"

void make_anim_obj(struct object* o, anim_mesh_t mesh) {}
void draw_object(struct object* o) {}
void draw_blur_object(struct object* o) {}
void free_object(struct object* o) {}
void load_shader_program(struct program* p, shader_t vert, shader_t frag) {}
void free_program(struct program* p) {}
void flip_fbo(fbo_t* fbo) {}
void swap_fbo(fbo_t* fbo) {}
void push_effect(effects_t* e, bound_t effect) {}

#include "character_internal.h"

bound_t make_hit_effect(struct stickman* sm) {return (bound_t){0};}
bound_t make_parry_effect(struct stickman* sm, float y) {return (bound_t){0};}
