//
//  shader.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "shader.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include "gl_core_3_3.h"
#include "file.h"

#ifdef DEBUG
static watch_t shader_watch = -1;
void poll_shader_changes()
{
    shader_t changed = poll_watch(shader_watch);
    if (changed)
        printf("%s\n", changed->name);
}
#endif

GLuint load_shader(shader_t shader)
{
    if (shader->shader)
        return shader->shader;
    
#ifdef DEBUG
    if (shader_watch == -1)
        shader_watch = make_watch();
    
    watch_file(shader_watch, shader->fname, shader);
#endif
    
    //create the shader object
    shader->shader = glCreateShader(shader->type);
    
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
    
    const GLchar* srcs[2] = { versionString, shader->source };
    
    glShaderSource(shader->shader, 2, srcs, NULL);
    glCompileShader(shader->shader);
    
    GLint status;
    glGetShaderiv(shader->shader, GL_COMPILE_STATUS, &status);
    if (!status) //compile failed
    {
        GLint infoLogLength;
        glGetShaderiv(shader->shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* infoLog = malloc_or_die(infoLogLength);
        
        //print error message
        glGetShaderInfoLog(shader->shader, infoLogLength, NULL, infoLog);
        printf("For %s:\n", shader->name);
        die(infoLog);
    }
    
    return shader->shader;
}

void load_shader_program(program_t* prog, shader_t vert, shader_t frag)
{
    //create our empty program Render
    GLuint program = glCreateProgram();
    
    GLuint vertShdr = load_shader(vert);
    GLuint fragShdr = load_shader(frag);
    
    //attach vertex and fragment shaders
    glAttachShader(program, vertShdr);
    glAttachShader(program, fragShdr);
    
    glBindAttribLocation(program, POSITION_ATTRIB, "position");
    glBindAttribLocation(program, POS_FROM_ATTRIB, "pos_from");
    glBindAttribLocation(program, POS_TO_ATTRIB, "pos_to");
    glBindAttribLocation(program, DERIV_FROM_ATTRIB, "deriv_from");
    glBindAttribLocation(program, DERIV_TO_ATTRIB, "deriv_to");
    
    glBindFragDataLocation(program, DRAW_BUFFER, "color");
    
    //link the program Render
    glLinkProgram(program);
    
    //check for linker errors
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* infoLog = malloc_or_die(infoLogLength);
        
        //print error message
        glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
        printf("For %s, %s:\n", vert->name, frag->name);
        die(infoLog);
    }
    
    glDetachShader(program, vertShdr);
    glDetachShader(program, fragShdr);
    
    prog->program = program;
    prog->camera = glGetUniformLocation(program, "camera");
    prog->transform = glGetUniformLocation(program, "transform");
    prog->time = glGetUniformLocation(program, "time");
    prog->pos_alpha = glGetUniformLocation(program, "pos_alpha");
    
    glUseProgram(program);
    GLuint framebuffer = glGetUniformLocation(program, "framebuffer");
    glUniform1i(framebuffer, FB_TEX_UNIT);
}

void free_program(program_t* prog)
{
    glDeleteProgram(prog->program);
}
