//
//  window.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/4/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "window.h"
#include "error.h"

#include "gl_core_3_3.h"
#include <stdio.h>

void error_callback(int error, const char* description) {
    printf("%s\n", description);
}

GLFWwindow* init_window() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        die("Could not initialize glfw");
    
    //only allow non-deprecated GL3.3 calls
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif
    
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Fighting Game", NULL, NULL);
    if (!window)
        die("Could not create window");
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    
    glfwMakeContextCurrent(window);
    //set vsync
    glfwSwapInterval(1);
    
    //glClearColor(1.f, 0.f, 1.f, 1.f);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
    
    return window;
}