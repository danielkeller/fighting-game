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
#include "world.h"
#include "math.h"
#include "gl_core_3_3.h"
#include "objects/test.h"
#include "stickman.h"
#include "shaders.h"
#include <math.h>

float camera_[] = {
    2., 0., 0.,
    0., 2., 0.,
    -1., -1., 1.
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
    
    object_t box;
    make_anim_obj(&box, test_verts, sizeof(test_verts), test_stride);
    anim_obj_keys(&box, &test_test_Basis_Key_1);
    
    stickman_t left, right;
    make_stickman(&left, &right, 1);
    make_stickman(&right, &left, -1);
    
    program_t simple;
    load_shader_program(&simple, anim_vert, waves_frag);
    
    /*
    GLint origin_unif = glGetUniformLocation(simple.program, "origin");
    GLint color_unif = glGetUniformLocation(simple.program, "main_color");
    GLint lead_color_unif = glGetUniformLocation(simple.program, "lead_color");
    */
     
    glUseProgram(simple.program);
    glUniformMatrix3fv(simple.camera, 1, GL_FALSE, camera.d);
    glUniformMatrix3fv(simple.transform, 1, GL_FALSE, eye3.d);
    
    init_game_time(&game_time);
    uint64_t load_total = 0, load_count = 0;
    
    while (!glfwWindowShouldClose(window))
    {
        usec_t frame_work_time = get_time() - game_time.last_render;
        
        //render_tick must be called immediately after the frame appears
        //onscreen for the algorithm to work.
        glfwSwapBuffers(window);
        float alpha = render_tick(&game_time);
        
        load_total += (frame_work_time * 100ull) / game_time.last_frame_length;
        ++load_count;
        if (load_count > 240) {
            printf("%llu%%\n", load_total / load_count);
            load_total = load_count = 0;
        }
        
        /* Poll for and process events */
        glfwPollEvents();
        
        while (phys_tick(&game_time)) {
            update_stickman(&left, &right, game_time.frame,
                            glfwGetKey(window, GLFW_KEY_Z),
                            glfwGetKey(window, GLFW_KEY_X));
            update_stickman(&right, &left, game_time.frame,
                            glfwGetKey(window, GLFW_KEY_COMMA),
                            glfwGetKey(window, GLFW_KEY_PERIOD));
        }
        
        if (game_time.last_frame_length > 20000ll)
            printf("%llu\n", game_time.last_frame_length / 1000ll);
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        fbo_window_size(&fbo, width, height);
        
        //check_fbo_status(&fbo);
        
        /* Render here */
        prepare_fbo(&fbo);
        
        draw_stickman(&left, game_time.frame, alpha);
        draw_stickman(&right, game_time.frame, alpha);
        
        /*
        flip_fbo(&fbo);
        
        glUseProgram(simple.program);
        
        float time = (float)game_time.current_time / 1000000.f;
        glUniform1f(simple.time, time);
        
        glUniform1f(simple.pos_alpha, sawf(time));
        glUniform3fv(color_unif, 1, blue);
        glUniform3fv(lead_color_unif, 1, black);
        glUniform2fv(origin_unif, 1, tl);
        
        glBindVertexArray(box.vertexArrayObject);
        
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
        
        flip_fbo(&fbo);
        
        glUniform1f(simple.pos_alpha, 1.f - sawf(time));
        glUniform3fv(color_unif, 1, red);
        glUniform3fv(lead_color_unif, 1, white);
        glUniform2fv(origin_unif, 1, bot);
        
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
        */
        /* Swap front and back buffers */
        blit_fbo(&fbo);
    }
    
    free_stickman(&left);
    free_stickman(&right);
    free_object(&box);
    free_program(&simple);
    
    free_fbo(&fbo);
    glfwTerminate();
    
    return 0;
}
