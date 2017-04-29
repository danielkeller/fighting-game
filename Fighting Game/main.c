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
#include "character.h"
#include "input.h"
#include <math.h>

float camera_[] = {
    2., 0., 0.,
    0., 2., 0.,
    -1., -1., 1.
};

int main (int argc, char* argv[]) {
    GLFWwindow* window = init_window();
    init_input(window);
    
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
        
        character_t left, right;
        make_stickman(&left, &right, RIGHT),
        make_stickman(&right, &left, LEFT);
        
        key_right = key_left = (key_events_t){0};
        
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
                poll_input();
                
                int both_alive
                = step_character(&left, key_left.move, SHIFT_FLAG(key_left.dodge), SHIFT_FLAG(key_left.attack))
                & step_character(&right, key_right.move, SHIFT_FLAG(key_right.dodge), SHIFT_FLAG(key_right.attack));
                
                if (!both_alive)
                    goto game_over;
                
                character_actions(&left);
                character_actions(&right);
            }
            
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            fbo_window_size(&fbo, width, height);
            calculate_camera((float)width, (float)height);
            
            poll_shader_changes();
            
            prepare_fbo(&fbo);
            
            draw_character(&left);
            draw_character(&right);
            draw_effects(&effects);
            
            blit_fbo(&fbo);
        }
        
    game_over:
        
        //Step the characters one more time so prev == next and they actually stop
        step_character(&left, 0, 0, 0);
        step_character(&right, 0, 0, 0);
        
        object_t game_over_text;
        make_object(&game_over_text, game_over_verts, sizeof(game_over_verts), 0);
        
        program_t game_over_shader;
        load_shader_program(&game_over_shader, simple_vert, color_frag);
        GLint color_unif = glGetUniformLocation(game_over_shader.program, "main_color");
        glUniform3f(color_unif, 1., 0., 0.);
        glUniformMatrix3fv(game_over_shader.camera, 1, GL_FALSE, camera.d);
        glUniformMatrix3fv(game_over_shader.transform, 1, GL_FALSE, eye3.d);
        
        while (!glfwWindowShouldClose(window) &&
               !(glfwGetKey(window, GLFW_KEY_ENTER) || (key_left.start && key_right.start)))
        {
            game_time.multiplier += 1;
            render_tick(&game_time);
            while (phys_tick(&game_time))
                ;
            
            prepare_fbo(&fbo);
            draw_character(&left);
            draw_character(&right);
            draw_effects(&effects);
            
            glUseProgram(game_over_shader.program);
            glBindVertexArray(game_over_text.vertexArrayObject);
            glDrawArrays(GL_TRIANGLES, 0, game_over_text.numVertecies);
            
            blit_fbo(&fbo);
            glfwSwapBuffers(window);
            glfwPollEvents();
            poll_shader_changes();
        }
        
        clear_effects(&effects);
        
        free_object(&game_over_text);
        free_program(&game_over_shader);
        
        free_character(&left);
        free_character(&right);
    }
    
    free_object(&box);
    free_effects(&effects);
    free_fbo(&fbo);
    glfwTerminate();
    
    return 0;
}
