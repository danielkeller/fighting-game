//
//  fbo.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/25/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "fbo.h"
#include "gl_core_3_3.h"
#include "error.h"
#include "shader.h"
#include "shaders.h"

//Pixelation level
static const int pixel = 1;

void make_fbo(fbo_t* fbo)
{
    glGenFramebuffers(2, fbo->fbos);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&fbo->default_fb);
    fbo->texes[0] = fbo->texes[1] = 0;
    fbo->width = fbo->height = 0;
    fbo->cur = 0;
    //Depth?
    
    for (size_t i = 0; i < 2; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbos[i]);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->default_fb);
    
    make_box(&fbo->quad);
    load_shader_program(&fbo->quad_shader, screenspace_vert, blit_frag);
}

void free_fbo(fbo_t* fbo)
{
    glDeleteFramebuffers(2, fbo->fbos);
    glDeleteTextures(2, fbo->texes);
}

#define FBO_ERROR_CASE(err) case err: die(#err);

void check_fbo_status(void)
{
    switch (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER))
    {
        FBO_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
        FBO_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
        FBO_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
        FBO_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
        FBO_ERROR_CASE(GL_FRAMEBUFFER_UNSUPPORTED)
        FBO_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
        FBO_ERROR_CASE(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
        default: die("Unknown framebuffer error");
        case GL_FRAMEBUFFER_COMPLETE: ; //OK
    }
}

void fbo_window_size(fbo_t* fbo, GLsizei width, GLsizei height)
{
    if (width == fbo->width && height == fbo->height)
        return;
    fbo->width = width, fbo->height = height;
    
    glDeleteTextures(2, fbo->texes);
    glGenTextures(2, fbo->texes);
    for (size_t i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE, fbo->texes[i]);
        //https://www.opengl.org/wiki/Common_Mistakes#Creating_a_complete_texture
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, width/pixel, height/pixel, 0,
                     GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbos[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             fbo->texes[i], 0);
        
        check_fbo_status();
    }
    glViewport(0, 0, width/pixel, height/pixel);
}

void prepare_fbo(fbo_t* fbo)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->fbos[fbo->cur]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void do_blit(fbo_t* fbo)
{
    glUseProgram(fbo->quad_shader.program);
    glBindVertexArray(fbo->quad.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, fbo->quad.numVertecies);
}

void swap_fbo(fbo_t* fbo)
{
    fbo->cur = 1 - fbo->cur;
    //Bind this to write in the framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->fbos[fbo->cur]);
    
    //Bind this to read in the shader
    glActiveTexture(GL_TEXTURE0 + FB_TEX_UNIT);
    glBindTexture(GL_TEXTURE_RECTANGLE, fbo->texes[1 - fbo->cur]);
}

void flip_fbo(fbo_t* fbo)
{
    swap_fbo(fbo);
    do_blit(fbo);
}

void blit_fbo(fbo_t* fbo)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->default_fb);
    glActiveTexture(GL_TEXTURE0 + FB_TEX_UNIT);
    glBindTexture(GL_TEXTURE_RECTANGLE, fbo->texes[fbo->cur]);
    do_blit(fbo);
}
