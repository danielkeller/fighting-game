//
//  main.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/4/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include <stdio.h>

#include "window.h"
#include "engine.h"
#include "objects/test.h"
#include "stickman.h"
#include <math.h>

const GLubyte* gluErrorString(GLenum errorCode);

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

struct key_events {
    int attack, dodge;
} key_left, key_right;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT) return;
    
    //Robust key-down detection
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_X) key_left.attack++;
    if (key == GLFW_KEY_PERIOD) key_right.attack++;
    if (key == GLFW_KEY_Z) key_left.dodge++;
    if (key == GLFW_KEY_COMMA) key_right.dodge++;
}

int main (int argc, char* argv[]) {
    GLFWwindow* window = init_window();
    glfwSetKeyCallback(window, key_callback);
    
    //make_anim_obj(&box, test_verts, sizeof(test_verts), test_stride);
    //anim_obj_keys(&box, &test_test_Basis_Key_1);
    /*
    program_t simple;
    load_shader_program(&simple, anim_vert, waves_frag);
    

    GLint origin_unif = glGetUniformLocation(simple.program, "origin");
    GLint color_unif = glGetUniformLocation(simple.program, "main_color");
    GLint lead_color_unif = glGetUniformLocation(simple.program, "lead_color");

     
    glUseProgram(simple.program);
    glUniformMatrix3fv(simple.camera, 1, GL_FALSE, camera.d);
    glUniformMatrix3fv(simple.transform, 1, GL_FALSE, eye3.d);
        */
    
    make_box(&box);
    make_fbo(&fbo);
    make_effects(&effects);
    
    stickman_t left, right;
    make_stickman(&left, &right.character, RIGHT);
    make_stickman(&right, &left.character, LEFT);
    
    /*
    uint64_t load_total = 0;
    int64_t frame_count = 0;
    usec_t last_fps = get_time();
    */
    
    init_game_time(&game_time);
    
    while (!glfwWindowShouldClose(window))
    {
        //usec_t frame_work_time = get_time() - game_time.last_render;
        
        //render_tick must be called immediately after the frame appears
        //onscreen for the algorithm to work.
        glfwSwapBuffers(window);
        render_tick(&game_time);
        /*
        ++frame_count;
        load_total += (frame_work_time * 100ull) / game_time.last_frame_length;
        usec_t elapsed = get_time() - last_fps;
        if (elapsed > 4000000ll)
        {
            printf("%llu%% ", load_total / frame_count);
            printf("%lld fps %lld uspf\n", (frame_count * 1000) / (elapsed / 1000), elapsed / frame_count);
            load_total = 0;
            frame_count = 0;
            last_fps += elapsed;
        }
        */
        
        while (phys_tick(&game_time)) {
            glfwPollEvents();
            
            step_character(&left.character, SHIFT_FLAG(key_left.dodge), SHIFT_FLAG(key_left.attack));
            step_character(&right.character, SHIFT_FLAG(key_right.dodge), SHIFT_FLAG(key_right.attack));
            stickman_actions(&left);
            stickman_actions(&right);
        }
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        fbo_window_size(&fbo, width, height);
        
        poll_shader_changes();
        
        prepare_fbo(&fbo);
        
        draw_stickman(&left);
        draw_stickman(&right);
        draw_effects(&effects);
        
        /*
        flip_fbo(&fbo);
        
        glUseProgram(simple.program);
        glBindVertexArray(box.vertexArrayObject);
        
        float time = secondsf(game_time.current_time);
        glUniform1f(simple.time, time);
        
        glUniform1f(simple.pos_alpha, sawf(time));
        glUniform3fv(color_unif, 1, blue);
        glUniform3fv(lead_color_unif, 1, black);
        glUniform2fv(origin_unif, 1, tl);
        
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);

        flip_fbo(&fbo);
        
        glUseProgram(simple.program);
        glBindVertexArray(box.vertexArrayObject);
        glUniform1f(simple.pos_alpha, 1.f - sawf(time));
        glUniform3fv(color_unif, 1, red);
        glUniform3fv(lead_color_unif, 1, white);
        glUniform2fv(origin_unif, 1, bot);
        
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
        */
        
        blit_fbo(&fbo);
        
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            printf("%s\n", gluErrorString(err));
        }
    }
    
    free_stickman(&left);
    free_stickman(&right);
    free_object(&box);
    //free_program(&simple);
    
    free_effects(&effects);
    free_fbo(&fbo);
    glfwTerminate();
    
    return 0;
}
