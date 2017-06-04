//
//  shader.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "engine.h"
#include "error.h"

#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>
#include "file.h"

static watch_t shader_watch = -1;

//Does no bounds checking
void add_program(struct program* program);
void remove_program(struct program* program);
#endif

void load_shader(shader_t shader);

void compile_shader(shader_t shader_data)
{
    //create the shader object
    GLuint shader = glCreateShader(shader_data->type);
    
    //attach and compile the source
    
    static const char versionString[] =
#ifdef __APPLE__
    "#version 330\n"
    "#line 1\n";
#else
    "#version 130\n"
    "#extension GL_ARB_uniform_buffer_object : require\n"
    "#line 0\n";
#endif
    
    const GLchar* srcs[2] = { versionString, shader_data->source };
    
    glShaderSource(shader, 2, srcs, NULL);
    glCompileShader(shader);
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) { //compile failed
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* infoLog = malloc(infoLogLength);
        
        //print error message
        glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
        printf("Compilation failed for %s:\n%s", shader_data->name, infoLog);
        
        glDeleteShader(shader);
        shader_data->shader = 0;
    }
    else
        shader_data->shader = shader;
}

void link_shader_program(struct program* prog)
{
    //create our empty program
    GLuint program = glCreateProgram();
    
    //attach vertex and fragment shaders
    glAttachShader(program, prog->vert->shader);
    glAttachShader(program, prog->frag->shader);
    
    load_shader(lib_frag);
    glAttachShader(program, lib_frag->shader);
    load_shader(lib_vert);
    glAttachShader(program, lib_vert->shader);
    
    glBindAttribLocation(program, POSITION_ATTRIB, "position");
    glBindAttribLocation(program, BONE_ATTRIB, "bone");
    glBindAttribLocation(program, PARENT_ATTRIB, "parent");
    glBindAttribLocation(program, WEIGHT_ATTRIB, "weight");
    glBindAttribLocation(program, BLUR_ALPHA_ATTRIB, "blur_alpha");
    
    glBindFragDataLocation(program, DRAW_BUFFER, "color");
    
    glLinkProgram(program);
    
    //check for linker errors
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* infoLog = malloc(infoLogLength);
        
        //print error message
        glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
        printf("For %s, %s:\n%s", prog->vert->name, prog->frag->name, infoLog);
        
        glDeleteProgram(program);
        prog->program = 0;
        return;
    }
    
    prog->program = program;
    prog->camera = glGetUniformLocation(program, "camera");
    prog->transform = glGetUniformLocation(program, "transform");
    prog->time = glGetUniformLocation(program, "time");
    prog->alpha = glGetUniformLocation(program, "alpha");
    prog->bones_from = glGetUniformLocation(program, "bones_from");
    prog->bones_to = glGetUniformLocation(program, "bones_to");
    
    glUseProgram(program);
    GLuint framebuffer = glGetUniformLocation(program, "framebuffer");
    glUniform1i(framebuffer, FB_TEX_UNIT);
}

void load_shader(shader_t shader)
{
    if (shader->shader)
        return;
    
#ifdef DEBUG
    if (shader_watch == -1)
        shader_watch = make_watch();
    
    watch_file(shader_watch, shader->fname, shader);
#endif
    
    compile_shader(shader);
    if (!shader->shader)
        die("Could not load shader");
}

//Right now the way shader types are specified is redundant, both in the file extention
//and the positions of the arguments. The extra info in the file extension could be
//useful in the future, if geometry shaders (for instance) are needed.
void load_shader_program(struct program* prog, shader_t vert, shader_t frag)
{
    prog->vert = vert, prog->frag = frag;
    
    load_shader(vert);
    load_shader(frag);
    
#ifdef DEBUG
    add_program(prog);
#endif
    
    link_shader_program(prog);
    if (!prog->program)
        die("Could not link shader program");
}

void free_program(struct program* prog)
{
#ifdef DEBUG
    remove_program(prog);
#endif
    glDeleteProgram(prog->program);
}

#ifdef DEBUG
static struct program* all_programs[1024] = {0};

void add_program(struct program* program)
{
    size_t i = 0;
    while (all_programs[i]) ++i;
    all_programs[i] = program;
}

void remove_program(struct program* program)
{
    size_t i = 0;
    while (all_programs[i] != program) ++i;
    
    for (; all_programs[i+1]; ++i)
        all_programs[i] = all_programs[i+1];
}

void update_program(struct program* program);

void poll_shader_changes()
{
    shader_t changed = poll_watch(shader_watch);
    if (!changed)
        return;
    
    printf("Updating shader %s...\n", changed->name);
    struct shader new = *changed;
    
    new.source = read_file(changed->fname);
    compile_shader(&new);
    free((void*)new.source);
    new.source = 0;
    
    if (!new.shader) //Compile error
        return;
    
    glDeleteShader(changed->shader);
    *changed = new;
    
    //Now find programs using this shader
    int updated = 0;
    
    for (size_t i = 0; all_programs[i]; ++i)
        if (changed == lib_vert || changed == lib_frag
            || all_programs[i]->vert == changed || all_programs[i]->frag == changed)
            update_program(all_programs[i]), ++updated;
    
    printf("Updated %d programs.\n", updated);
}

void update_program(struct program* program)
{
    //Try re-linking the program with the new shaders
    struct program new_program = *program;
    link_shader_program(&new_program);
    
    if (!new_program.program)
        return;
    
    glUseProgram(new_program.program);
    
    //Copy everything over
    GLint n_unifs;
    glGetProgramiv(new_program.program, GL_ACTIVE_UNIFORMS, &n_unifs);
    
    const GLsizei buf_size = 32; // maximum name length
    GLchar name[buf_size]; // variable name in GLSL
    GLint old_size, new_size, old_location;
    GLenum old_type, new_type;
    
    for (GLint new_location = 0; new_location < n_unifs; new_location++) {
        glGetActiveUniform(new_program.program, (GLuint)new_location, buf_size, NULL, &new_size, &new_type, name);
        
        old_location = glGetUniformLocation(program->program, name);
        if (old_location == -1)
            continue;
        
        glGetActiveUniform(program->program, old_location, buf_size, NULL, &old_size, &old_type, name);
        
        if (old_location != new_location)
            printf("Warning: Uniform %s moved\n", name);
        
        if (new_size != old_size || new_type != old_type)
            continue;
        
        #define COPY_UNIF(glty, type, elems, suffix) \
            if (new_type == glty) { \
                type val[elems*new_size]; \
                glGetUniform##suffix##v(program->program, old_location, val); \
                glUniform##elems##suffix##v(new_location, new_size, val); \
            }
        #define COPY_UNIF_MAT(glty, type, rows, suffix) \
            if (new_type == glty) { \
                type val[rows*rows*new_size]; \
                glGetUniform##suffix##v(program->program, old_location, val); \
                glUniformMatrix##rows##suffix##v(new_location, new_size, GL_FALSE, val); \
            }
        
        COPY_UNIF(GL_SAMPLER_2D, GLint, 1, i)
        COPY_UNIF(GL_FLOAT, GLfloat, 1, f)
        COPY_UNIF(GL_FLOAT_VEC2, GLfloat, 2, f)
        COPY_UNIF(GL_FLOAT_VEC3, GLfloat, 3, f)
        COPY_UNIF(GL_FLOAT_VEC4, GLfloat, 4, f)
        COPY_UNIF_MAT(GL_FLOAT_MAT2, GLfloat, 2, f)
        COPY_UNIF_MAT(GL_FLOAT_MAT3, GLfloat, 3, f)
        COPY_UNIF_MAT(GL_FLOAT_MAT4, GLfloat, 4, f)
        
        #undef COPY_UNIF
        #undef COPY_UNIF_MAT
    }
    //Could potentially copy attribute locations as well
    
    glDeleteProgram(program->program);
    *program = new_program;
}

#endif
