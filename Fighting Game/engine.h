//
//  engine.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/20/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef engine_h
#define engine_h
#include "types.h"
#include "function.h"
#include "gl_core_3_3.h"

//*** Math
typedef struct Mat3 {
    float d[9];
} Mat3;

extern Mat3 eye3;

Mat3 affine(float theta, float x, float y);

//*** Object
void make_box(object_t*);
void make_anim_obj(object_t*, float* verts, GLsizei verts_sz, GLsizei stride);
void anim_obj_keys(object_t*, const anim_step_t* step);
void free_object(object_t*);

//*** Shader
#define POSITION_ATTRIB 0
#define POS_FROM_ATTRIB 0
#define POS_TO_ATTRIB 1
#define DERIV_FROM_ATTRIB 3
#define DERIV_TO_ATTRIB 4
#define DRAW_BUFFER 0
#define FB_TEX_UNIT 0

struct shader {
    GLuint shader;
    const char* name;
    GLenum type;
    const char* source;
    
#ifdef DEBUG
    const char* fname;
#endif
};

#include "shaders.h"

void load_shader_program(program_t*, shader_t vert, shader_t frag);
void free_program(program_t*);

#ifdef DEBUG
//This assumes the program objects don't move around in memory
void poll_shader_changes();
#else
inline void poll_shader_changes() {}
#endif

//*** FBO
typedef struct fbo
{
    GLsizei width, height;
    GLuint fbos[2], default_fb;
    GLuint texes[2];
    size_t cur;
    object_t quad;
    program_t quad_shader;
} fbo_t;

//Note: fbo_window_size must be called before the fbo can be used
void make_fbo(fbo_t* fbo);

//Note: prepare_fbo must be called before the fbo can be used
void fbo_window_size(fbo_t* fbo, GLsizei width, GLsizei height);

//Clears buffers. Note: changes read and draw fb bindings
void prepare_fbo(fbo_t* fbo);

//Note: changes read and draw fb bindings
void flip_fbo(fbo_t* fbo);

//same as flip_fbo, but doesn't copy pixels
void swap_fbo(fbo_t* fbo);

//Note: changes read and draw fb bindings
void blit_fbo(fbo_t* fbo);

void free_fbo(fbo_t* fbo);

//*** Time
//25 FPS
static const usec_t tick_length = 40000ll;
//4 FPS
static const usec_t frame_limit = 250000ll;

typedef struct game_time {
    unsigned long long frame;
    float alpha;
    usec_t last_render, last_frame_length, current_time, unsimulated_time;
} game_time_t;

void init_game_time(game_time_t*);

usec_t get_time(void);

//Update the game_time for one physics tick, and return 1 if we should simulate
int phys_tick(game_time_t*);
//Update the game_time for one render tick, and return the leftover time as a fraction
//of the tick length; [0,1).
void render_tick(game_time_t*);

//*** Effects
#define MAX_EFFECTS 128

typedef struct effects {
    int num_effects;
    bound_t effects[MAX_EFFECTS];
} effects_t;

void make_effects(effects_t*);

//Takes ownership of effect
void push_effect(effects_t*, bound_t effect);

void draw_effects(effects_t*);
void free_effects(effects_t*);

//*** Globals
extern Mat3 camera;
extern fbo_t fbo;
extern game_time_t game_time;
extern object_t box;
extern effects_t effects;

#endif /* engine_h */