#include "screen_effects.h"

void make_screen_effects(struct screen_effects* se)
{
    load_shader_program(&se->screen_noise, screenspace_vert, noise_frag);
    load_shader_program(&se->blur, screenspace_vert, blur_frag);
    GLint fb1 = glGetUniformLocation(se->blur.program, "noise");
    glUniform1i(fb1, FB_TEX_UNIT+1);
    make_fbo(&se->noise_fbo);
}

void draw_screen_effects(struct screen_effects* se)
{
    fbo_size(&se->noise_fbo, fbos.fbos[0].width/2, fbos.fbos[0].height/2);
    
    draw_fbo(&se->noise_fbo);
    glUseProgram(se->screen_noise.program);
    glUniform1f(se->screen_noise.time, (float)game_time.current_time / 1000000.f);
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
    
    swap_fbos(&fbos);
    glUseProgram(se->blur.program);
    glActiveTexture(GL_TEXTURE0 + FB_TEX_UNIT + 1);
    glBindTexture(GL_TEXTURE_RECTANGLE, se->noise_fbo.tex);
    glBindVertexArray(box.vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void free_screen_effects(struct screen_effects* se)
{
    free_program(&se->screen_noise);
    free_program(&se->blur);
    free_fbo(&se->noise_fbo);
}
