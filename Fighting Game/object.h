//
//  object.h
//  Fighting Game
//
//  Created by Daniel Keller on 8/20/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef object_h
#define object_h
#include "gl_types.h"

typedef struct object
{
    GLuint vertexArrayObject;
    GLuint vertexBufferObject;
    GLsizei numVertecies;
    GLsizei stride;
} object_t;

void free_object(object_t*);

void make_box(object_t*);

typedef struct animation
{
    int id;
    GLsizei from, to;
    int frames;
} animation_t;

void make_anim_obj(object_t*, float* verts, GLsizei verts_sz, GLsizei stride);
void anim_obj_keys(object_t*, GLsizei from_offset, GLsizei to_offset);

#endif /* object_h */
