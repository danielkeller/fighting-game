//
//  shader.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "shader.h"
#include "file.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include "gl_core_3_3.h"

GLuint load_shader(GLenum eShaderType, const char* name)
{
    size_t file_len;
    char* file_text = read_file(name, &file_len);
    
    //create the shader object
    GLuint shader = glCreateShader(eShaderType);
    
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
    
    GLint lens[2] = {sizeof(versionString) - 1, (GLint)file_len};
    const GLchar* srcs[2] = { versionString, file_text };
    
    glShaderSource(shader, 2, srcs, lens);
    glCompileShader(shader);
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) //compile failed
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* infoLog = malloc_or_die(infoLogLength);
        
        //print error message
        glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
        printf("For %s:\n", name);
        die(infoLog);
    }
    
    free(file_text);
    return shader;
}

void load_shader_program(program_t* prog, const char* vert_name, const char* frag_name)
{
    //create our empty program Render
    GLuint program = glCreateProgram();
    
    GLuint vertShdr = load_shader(GL_VERTEX_SHADER, vert_name);
    GLuint fragShdr = load_shader(GL_FRAGMENT_SHADER, frag_name);
    
    //attach vertex and fragment shaders
    glAttachShader(program, vertShdr);
    glAttachShader(program, fragShdr);
    
    glBindAttribLocation(program, POSITION_ATTRIB, "position");
    
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
        printf("For %s, %s:\n", vert_name, frag_name);
        die(infoLog);
    }
    
    //shaders are no longer used now that the program is linked
//#ifdef NDEBUG
    glDetachShader(program, vertShdr);
    glDeleteShader(vertShdr);
    glDetachShader(program, fragShdr);
    glDeleteShader(fragShdr);
//#endif
    
    prog->program = program;
    prog->camera = glGetUniformLocation(program, "camera");
    prog->transform = glGetUniformLocation(program, "transform");
    prog->time = glGetUniformLocation(program, "time");
}

void free_program(program_t* prog)
{
    glDeleteProgram(prog->program);
}