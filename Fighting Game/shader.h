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
#define DERIV_FROM_ATTRIB 3
#define DERIV_TO_ATTRIB 4
#define DRAW_BUFFER 0
#define FB_TEX_UNIT 0


typedef struct program
{
    GLuint program;
    GLint transform, camera, time, pos_alpha;
} program_t;

struct shader {
    GLuint shader;
    const char* name;
    GLenum type;
    const char* source;
#ifdef DEBUG
    const char* fname;
    program_t *used_in[128];
#endif
};
typedef struct shader* shader_t;

void load_shader_program(program_t*, shader_t vert, shader_t frag);
void free_program(program_t*);

#ifdef DEBUG
void poll_shader_changes();
#else
inline void poll_shader_changes() {}
#endif

#endif /* shader_h */
