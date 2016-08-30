//
//  main.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/4/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include <stdio.h>

#include "window.h"
#include "fbo.h"
#include "object.h"
#include "shader.h"
#include "time.h"
#include "gl_core_3_3.h"
#include "objects/test.h"
#include <math.h>

float eye3[] = {
    1., 0., 0.,
    0., 1., 0.,
    0., 0., 1.
};
float camera_[] = {
    2., 0., 0.,
    0., 2., 0.,
    -1., -1., 1.
};
float camera[] = {
    .5, 0., 0.,
    0., .5, 0.,
    0., 0., 1.
};

float black[] = {0., 0., 0.};
float red[] = {1., 0., 0.};
float blue[] = {0., 1., 1.};
float green[] = {0., 1., 0.};
float white[] = {1., 1., 1.};
float bot[] = {0.5, -1.};
float tl[] = {M_SQRT1_2, M_SQRT1_2};

float sawf(float v)
{
    return 1.f - fabsf(fmodf(v, 2.f) - 1.f);
}

int main (int argc, char* argv[]) {
    GLFWwindow* window = init_window();
    
    fbo_t fbo;
    make_fbo(&fbo);
    
    game_time_t game_time;
    
    program_t simple;
    object_t box;
    load_shader_program(&simple, "assets/anim.vert", "assets/simple.frag");
    make_anim_obj(&box, test_verts, sizeof(test_verts), test_stride);
    anim_obj_keys(&box, test_Basis, test_Key_1);
    GLint alpha_unif = glGetUniformLocation(simple.program, "pos_alpha");
    
    GLint origin_unif = glGetUniformLocation(simple.program, "origin");
    GLint color_unif = glGetUniformLocation(simple.program, "main_color");
    GLint lead_color_unif = glGetUniformLocation(simple.program, "lead_color");
    
    glBindVertexArray(box.vertexArrayObject);
    glUseProgram(simple.program);
    glUniformMatrix3fv(simple.camera, 1, GL_FALSE, camera);
    glUniformMatrix3fv(simple.transform, 1, GL_FALSE, eye3);
    //glUniform1f(alpha_unif, 0.f);
    
    init_game_time(&game_time);
    
    while (!glfwWindowShouldClose(window))
    {
        while (phys_tick(&game_time))
            ; //TODO: physics
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        fbo_window_size(&fbo, width, height);
        
        //check_fbo_status(&fbo);
        
        /* Render here */
        flip_fbo(&fbo);
        glClear(GL_COLOR_BUFFER_BIT);
        flip_fbo(&fbo);
        glClear(GL_COLOR_BUFFER_BIT);
        
        float time = (float)game_time.current_time / 1000000.f;
        glUniform1f(simple.time, time);
        
        glUniform1f(alpha_unif, sawf(time));
        glUniform3fv(color_unif, 1, blue);
        glUniform3fv(lead_color_unif, 1, black);
        glUniform2fv(origin_unif, 1, tl);
        
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
        
        flip_fbo(&fbo);
        
        glUniform1f(alpha_unif, 1.f - sawf(time));
        glUniform3fv(color_unif, 1, red);
        glUniform3fv(lead_color_unif, 1, white);
        glUniform2fv(origin_unif, 1, bot);
        
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
        
        /* Swap front and back buffers */
        blit_fbo(&fbo);
        glfwSwapBuffers(window);
        render_tick(&game_time);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    free_object(&box);
    free_program(&simple);
    
    free_fbo(&fbo);
    glfwTerminate();
    
    return 0;
}