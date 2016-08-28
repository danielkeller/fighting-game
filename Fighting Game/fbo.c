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

//Pixelation level
static const int pixel = 1;

void make_fbo(fbo_t* fbo)
{
    glGenFramebuffers(1, &fbo->fbo);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&fbo->default_fb);
    fbo->texes[0] = fbo->texes[1] = 0;
    fbo->width = fbo->height = 0;
    fbo->cur_tex = 0;
    //Depth?
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->default_fb);
}

void free_fbo(fbo_t* fbo)
{
    glDeleteFramebuffers(1, &fbo->fbo);
    glDeleteTextures(2, fbo->texes);
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
        glBindTexture(GL_TEXTURE_2D, fbo->texes[i]);
        //https://www.opengl.org/wiki/Common_Mistakes#Creating_a_complete_texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width/pixel, height/pixel, 0,
                     GL_RGBA, GL_FLOAT, NULL);
    }
    glViewport(0, 0, width/pixel, height/pixel);
}

#define FBO_ERROR_CASE(err) case err: die(#err);

void check_fbo_status(fbo_t* fbo)
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
        case 0: die("Unknown framebuffer error");
        default: ;
    }
}

void flip_fbo(fbo_t* fbo)
{
    fbo->cur_tex = 1 - fbo->cur_tex;
    //Bind this to write in the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, fbo->texes[fbo->cur_tex], 0);
    
    //Bind this to read in the shader
    glActiveTexture(GL_TEXTURE0 + FB_TEX_UNIT);
    glBindTexture(GL_TEXTURE_2D, fbo->texes[1 - fbo->cur_tex]);
}

void blit_fbo(fbo_t* fbo)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->default_fb);
    glBlitFramebuffer(0, 0, fbo->width/pixel, fbo->height/pixel,
                      0, 0, fbo->width, fbo->height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->default_fb);
}
