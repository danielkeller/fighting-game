//
//  shader.h
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef shader_h
#define shader_h
#include "gl_types.h"

#define POSITION_ATTRIB 0
#define DRAW_BUFFER 0
#define FB_TEX_UNIT 0

typedef struct program
{
    GLuint program;
    GLint transform, camera, time;
} program_t;

void load_shader_program(program_t*, const char* vert_name, const char* frag_name);
void free_program(program_t*);

#endif /* shader_h */
