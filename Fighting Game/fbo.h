//
//  fbo.h
//  Fighting Game
//
//  Created by Daniel Keller on 8/25/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef fbo_h
#define fbo_h
#include "gl_types.h"
#include "object.h"
#include "shader.h"

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

//Note: changes read and draw fb bindings
void blit_fbo(fbo_t* fbo);

void free_fbo(fbo_t* fbo);

#endif /* fbo_h */
