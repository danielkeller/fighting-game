//
//  fbo.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/25/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "engine.h"
#include "error.h"
#include "shaders.h"

void check_fbo_status(void)
{
#define FBO_ERROR_CASE(err) case err: die(#err);
    switch (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)) {
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
#undef FBO_ERROR_CASE
}

void do_blit(fbos_t* fbos)
{
    glUseProgram(fbos->quad_shader.program);
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void make_fbo(fbo_t* fbo)
{
    glGenFramebuffers(1, &fbo->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    fbo->tex = fbo->depth = 0;
    fbo->width = fbo->height = 0;
}

void free_fbo(fbo_t* fbo)
{
    glDeleteFramebuffers(1, &fbo->fbo);
    glDeleteTextures(1, &fbo->tex);
    glDeleteRenderbuffers(1, &fbo->depth);
}

void fbo_size(fbo_t* fbo, GLsizei width, GLsizei height)
{
    if (width == fbo->width && height == fbo->height)
        return;
    fbo->width = width; fbo->height = height;
    
    glDeleteTextures(1, &fbo->tex);
    glGenTextures(1, &fbo->tex);
    
    glBindTexture(GL_TEXTURE_RECTANGLE, fbo->tex);
    //https://www.opengl.org/wiki/Common_Mistakes#Creating_a_complete_texture
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA8, width, height, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo->tex, 0);
    
    glDeleteRenderbuffers(1, &fbo->depth);
    glGenRenderbuffers(1, &fbo->depth);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo->depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->depth);
    
    check_fbo_status();
}

void read_fbo(fbo_t* fbo)
{
    glActiveTexture(GL_TEXTURE0 + FB_TEX_UNIT);
    glBindTexture(GL_TEXTURE_RECTANGLE, fbo->tex);
}

void draw_fbo(fbo_t* fbo)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->fbo);
    glViewport(0, 0, fbo->width, fbo->height);
}

void make_fbos(fbos_t* fbos)
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&fbos->default_fb);
    fbos->cur = 0;
    
    for (size_t i = 0; i < 2; ++i)
        make_fbo(&fbos->fbos[i]);
    
    load_shader_program(&fbos->quad_shader, screenspace_vert, blit_frag);
}

void free_fbos(fbos_t* fbos)
{
    for (size_t i = 0; i < 2; ++i)
        free_fbo(&fbos->fbos[i]);
}

void fbos_window_size(fbos_t* fbos, GLsizei width, GLsizei height)
{
    for (size_t i = 0; i < 2; ++i)
        fbo_size(&fbos->fbos[i], width, height);
}

void prepare_fbos(fbos_t* fbos)
{
    draw_fbo(&fbos->fbos[fbos->cur]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void swap_fbos(fbos_t* fbos)
{
    fbos->cur = 1 - fbos->cur;
    draw_fbo(&fbos->fbos[fbos->cur]);
    read_fbo(&fbos->fbos[1 - fbos->cur]);
}

void flip_fbos(fbos_t* fbos)
{
    swap_fbos(fbos);
    do_blit(fbos);
}

void blit_other_fbo(fbos_t* fbos, fbo_t* fbo)
{
    read_fbo(fbo);
    draw_fbo(&fbos->fbos[fbos->cur]);
    do_blit(fbos);
}

void blit_fbos(fbos_t* fbos)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos->default_fb);
    read_fbo(&fbos->fbos[fbos->cur]);
    do_blit(fbos);
}
