//
//  main.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/4/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include <stdio.h>

#include "window.h"
#include "object.h"
#include "shader.h"
#include "time.h"
#include "gl_core_3_3.h"
#include <math.h>

float eye3[] = {
    1., 0., 0.,
    0., 1., 0.,
    0., 0., 1.
};
float camera[] = {
    2., 0., 0.,
    0., 2., 0.,
    -1., -1., 1.
};

float black[] = {0., 0., 0.};
float red[] = {1., 0., 0.};
float blue[] = {0., 1., 1.};
float white[] = {1., 1., 1.};
float bot[] = {0.5, -1.};
float tl[] = {M_SQRT1_2, M_SQRT1_2};

int main (int argc, char* argv[]) {
    GLFWwindow* window = init_window();
    
    glEnablei(GL_BLEND, DRAW_BUFFER);
    glClearDepth(0.);
    
    game_time_t game_time;
    
    program_t simple;
    object_t box;
    load_shader_program(&simple, "assets/simple.vert", "assets/simple.frag");
    make_box(&box);
    
    GLint origin_unif = glGetUniformLocation(simple.program, "origin");
    GLint color_unif = glGetUniformLocation(simple.program, "main_color");
    GLint lead_color_unif = glGetUniformLocation(simple.program, "lead_color");
    
    glBindVertexArray(box.vertexArrayObject);
    glUseProgram(simple.program);
    glUniformMatrix3fv(simple.camera, 1, GL_FALSE, camera);
    glUniformMatrix3fv(simple.transform, 1, GL_FALSE, eye3);
    
    init_game_time(&game_time);
    
    while (!glfwWindowShouldClose(window))
    {
        while (phys_tick(&game_time))
            ; //TODO: physics
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, (GLsizei) width, (GLsizei) height);
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float time = (float)game_time.current_time / 1000000.f;
        glUniform1f(simple.time, time);
        
        glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        
        glUniform3fv(color_unif, 1, blue);
        glUniform3fv(lead_color_unif, 1, black);
        glUniform2fv(origin_unif, 1, tl);
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
        
        glUniform3fv(color_unif, 1, red);
        glUniform3fv(lead_color_unif, 1, white);
        glUniform2fv(origin_unif, 1, bot);
        
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT);
        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ONE);
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
        
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA, GL_ONE, GL_DST_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, box.numVertecies);
        
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        render_tick(&game_time);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    free_object(&box);
    free_program(&simple);
    
    glfwTerminate();
    
    return 0;
}