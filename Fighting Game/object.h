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
} object_t;

void make_box(object_t*);
void free_object(object_t*);

#endif /* object_h */
