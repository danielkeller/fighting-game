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
#define POS_FROM_ATTRIB 0
#define POS_TO_ATTRIB 1
#define DRAW_BUFFER 0
#define FB_TEX_UNIT 0

struct shader {
    GLuint shader;
    const char* name;
    GLenum type;
    const char* source;
};
typedef struct shader* shader_t;

typedef struct program
{
    GLuint program;
    GLint transform, camera, time, pos_alpha;
} program_t;

void load_shader_program(program_t*, shader_t vert, shader_t frag);
void free_program(program_t*);

#endif /* shader_h */
