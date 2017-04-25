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
#include "objects/game over.h"
#include "stickman.h"
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
    
    make_box(&box);
    make_fbo(&fbo);
    make_effects(&effects);
    
    /*
    uint64_t load_total = 0;
    int64_t frame_count = 0;
    usec_t last_fps = get_time();
    */
    
    while (!glfwWindowShouldClose(window))
    {
        init_game_time(&game_time);
        
        stickman_t left, right;
        make_stickman(&left, &right.character, RIGHT);
        make_stickman(&right, &left.character, LEFT);
        
        key_right = key_left = (struct key_events){0};
        
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
            
            if (left.character.prev.health == 0 || right.character.prev.health == 0)
                break;
            
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            fbo_window_size(&fbo, width, height);
            
            poll_shader_changes();
            
            prepare_fbo(&fbo);
            
            draw_stickman(&left);
            draw_stickman(&right);
            draw_effects(&effects);
            
            blit_fbo(&fbo);
        }
        
        clear_effects(&effects);
        
        object_t game_over_text;
        make_object(&game_over_text, game_over_verts, sizeof(game_over_verts), 0);
        
        program_t game_over_shader;
        load_shader_program(&game_over_shader, simple_vert, color_frag);
        GLint color_unif = glGetUniformLocation(game_over_shader.program, "main_color");
        glUniform3f(color_unif, 1., 0., 0.);
        glUniformMatrix3fv(game_over_shader.camera, 1, GL_FALSE, camera.d);
        glUniformMatrix3fv(game_over_shader.transform, 1, GL_FALSE, eye3.d);
        
        while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ENTER))
        {
            glClear(GL_COLOR_BUFFER_BIT);
            draw_stickman(&left);
            draw_stickman(&right);
            
            glUseProgram(game_over_shader.program);
            glBindVertexArray(game_over_text.vertexArrayObject);
            glDrawArrays(GL_TRIANGLES, 0, game_over_text.numVertecies);
            
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        free_object(&game_over_text);
        free_program(&game_over_shader);
        
        free_stickman(&left);
        free_stickman(&right);
    }
    
    free_object(&box);
    free_effects(&effects);
    free_fbo(&fbo);
    glfwTerminate();
    
    return 0;
}
