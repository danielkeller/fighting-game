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
typedef unsigned long long frame_t;

typedef const struct mesh* mesh_t;
typedef const struct anim_mesh* anim_mesh_t;
typedef const struct animation* animation_t;

struct object
{
    GLuint vertexArrayObject;
    GLuint vertexBufferObject, indexBufferObject;
    GLsizei numVertecies;
};

typedef struct shader* shader_t;

struct program
{
    GLuint program;
    GLint transform, camera, time, bones_from, bones_to, alpha;
    
    shader_t vert, frag;
};

struct button {
    int down, pressed, cancelled;
    frame_t press_time;
};

struct key_events {
    struct button move, attack, special, dodge;
    int start;
};

#endif /* types_h */
