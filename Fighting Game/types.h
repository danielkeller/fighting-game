//
//  types.h
//  Fighting Game
//  Intended to be included into headers.
//
//  Created by Daniel Keller on 4/20/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef types_h
#define types_h
#include "gl_types.h"

typedef long long usec_t;

typedef struct object
{
    GLuint vertexArrayObject;
    GLuint vertexBufferObject;
    GLsizei numVertecies;
    GLsizei stride;
} object_t;

typedef struct anim_step {
    GLsizei p_from, p_to; //position
    GLsizei d_from, d_to; //derivative
} anim_step_t;

typedef struct shader* shader_t;

typedef struct program
{
    GLuint program;
    GLint transform, camera, time, pos_alpha;
    
    shader_t vert, frag;
} program_t;

#define SHIFT_FLAG(f) ((f) ? (f)--, 1 : 0)

#endif /* types_h */