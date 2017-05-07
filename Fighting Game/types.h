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

struct object
{
    GLuint vertexArrayObject;
    GLuint vertexBufferObject;
    GLsizei numVertecies;
    GLsizei stride;
};

struct anim_step {
    GLsizei p_from, p_to; //position
    GLsizei d_from, d_to; //derivative
};

typedef struct shader* shader_t;

struct program
{
    GLuint program;
    GLint transform, camera, time, pos_alpha;
    
    shader_t vert, frag;
};

struct button {
    int down, pressed;
};

#endif /* types_h */
