#include "screen_effects.h"

void make_screen_effects(struct screen_effects* se)
{
    load_shader_program(&se->screen_noise, screenspace_vert, noise_frag);
    load_shader_program(&se->blur, screenspace_vert, blur_frag);
    se->blur_intensity_unif = glGetUniformLocation(se->blur.program, "intensity");
    GLint fb1 = glGetUniformLocation(se->blur.program, "noise");
    glUniform1i(fb1, FB_TEX_UNIT+1);
    load_shader_program(&se->waves, screenspace_vert, waves_frag);
    se->waves_intensity_unif = glGetUniformLocation(se->waves.program, "intensity");
    make_fbo(&se->noise_fbo);
}

void draw_screen_effects(struct screen_effects* se, float intensity)
{
    fbo_size(&se->noise_fbo, fbos.fbos[0].width/2, fbos.fbos[0].height/2);
    glBindVertexArray(box.vertexArrayObject);
    
    swap_fbos(&fbos);
    glUseProgram(se->waves.program);
    glUniform1f(se->waves.time, (float)game_time.current_time / 1000000.f);
    glUniform1f(se->waves_intensity_unif, intensity);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
    
    draw_fbo(&se->noise_fbo);
    glUseProgram(se->screen_noise.program);
    glUniform1f(se->screen_noise.time, (float)game_time.current_time / 1000000.f);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
    
    swap_fbos(&fbos);
    glUseProgram(se->blur.program);
    glUniform1f(se->blur.time, (float)game_time.current_time / 1000000.f);
    glUniform1f(se->blur_intensity_unif, intensity);
    glActiveTexture(GL_TEXTURE0 + FB_TEX_UNIT + 1);
    glBindTexture(GL_TEXTURE_RECTANGLE, se->noise_fbo.tex);
    glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
}

void free_screen_effects(struct screen_effects* se)
{
    free_program(&se->screen_noise);
    free_program(&se->blur);
    free_program(&se->waves);
    free_fbo(&se->noise_fbo);
}
