//
//  main.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/4/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include <stdio.h>

#include "options.h"
#include "window.h"
#include "engine.h"
#include "objects/game over.h"
#include "character_internal.h" //FIXME
#include "screen_effects.h"
#include <math.h>

int main (int argc, char* argv[])
{
    GLFWwindow* window = init_window();
    init_input(window);
    
    make_box(&box);
    make_fbos(&fbos);
    make_effects(&effects);
    struct screen_effects screen_effects;
    make_screen_effects(&screen_effects);
    
#if PROFILE
    int64_t frame_count = 0, with_frame_count = 0, without_frame_count = 0;
    usec_t last_fps = get_time();
    int run_with = 0;
    #define TIME if (run_with)
#else
    #define TIME
#endif
    
    while (!glfwWindowShouldClose(window)) {
        init_game_time(&game_time);
        
        character_t
            *left = alloc_character(),
            *right = alloc_character();
        make_stickman(left, right, RIGHT);
        make_fatman(right, left, LEFT);
        
        key_right = key_left = (struct key_events){{0}};
        
        while (!glfwWindowShouldClose(window)) {
            while (phys_tick(&game_time)) {
                poll_input();
                
                int both_alive
                = step_character(left,  &key_left)
                & step_character(right, &key_right);
                
                if (!both_alive)
                    goto game_over;
                
                character_actions(left);
                character_actions(right);
            }
            
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            fbos_window_size(&fbos, width, height);
            calculate_camera((float)width, (float)height);
            
            poll_shader_changes();
            
            prepare_fbos(&fbos);
            
            draw_character(left);
            draw_character(right);
            draw_effects(&effects);
            TIME {
                int min_health = left->next.health < right->next.health ? left->next.health : right->next.health;
                draw_screen_effects(&screen_effects, 1.f - (float)min_health / 100.f);
            }
            
            blit_fbos(&fbos);
            glfwSwapBuffers(window);
            //render_tick must be called immediately after the frame appears
            //onscreen for the algorithm to work.
            render_tick(&game_time);
            
#if PROFILE
            if (run_with) ++with_frame_count;
            else ++without_frame_count;
            ++frame_count;
            
            usec_t finish = get_time();
            usec_t elapsed = finish - last_fps;
            run_with = elapsed & 0x20000ll; //Alternate exactly 32 times in one period
            if (elapsed > 0x400000ll) //About 4 seconds
            {
                usec_t frame_time = elapsed / frame_count;
                int pct = (float)frame_time / (1000000.f / 60.f) * 100.f;
                usec_t difference = elapsed / 2 / with_frame_count - elapsed / 2 / without_frame_count;
                printf("%lldus %d%% TIME(%lldus)\n", frame_time, pct, difference);
                frame_count = 0;
                with_frame_count = 0;
                without_frame_count = 0;
                last_fps = finish;
            }
#endif
        }
        
    game_over:
        
        //Step the characters one more time so prev == next and they actually stop
        step_character(left, &no_key_events);
        step_character(right, &no_key_events);
        
        struct object game_over_text;
        make_object(&game_over_text, text);
        
        struct program game_over_shader;
        load_shader_program(&game_over_shader, simple_vert, color_frag);
        GLint color_unif = glGetUniformLocation(game_over_shader.program, "main_color");
        glUniform3f(color_unif, 1., 0., 0.);
        glUniformMatrix3fv(game_over_shader.camera, 1, GL_FALSE, camera.d);
        glUniformMatrix3fv(game_over_shader.transform, 1, GL_FALSE, eye3.d);
        
        while (!glfwWindowShouldClose(window) &&
               !(glfwGetKey(window, GLFW_KEY_ENTER) || (key_left.start && key_right.start))) {
            game_time.multiplier += 1;
            render_tick(&game_time);
            while (phys_tick(&game_time))
                ;
            
            prepare_fbos(&fbos);
            draw_character(left);
            draw_character(right);
            draw_effects(&effects);
            draw_screen_effects(&screen_effects, 0.);
            
            glUseProgram(game_over_shader.program);
            glBindVertexArray(game_over_text.vertexArrayObject);
            glDrawArrays(GL_TRIANGLES, 0, game_over_text.numVertecies);
            
            blit_fbos(&fbos);
            glfwSwapBuffers(window);
            poll_input();
            poll_shader_changes();
        }
        
        clear_effects(&effects);
        
        free_object(&game_over_text);
        free_program(&game_over_shader);
        
        free_character(left);
        free_character(right);
    }
    
    free_screen_effects(&screen_effects);
    free_object(&box);
    free_effects(&effects);
    free_fbos(&fbos);
    glfwTerminate();
    
    return 0;
}
