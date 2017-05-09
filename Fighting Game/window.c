//
//  window.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/4/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "window.h"
#include "error.h"
#include "options.h"

#include "gl_core_3_3.h"
#include <stdio.h>

static const char* window_title = "Fighting Game";

void error_callback(int error, const char* description)
{
    printf("%s\n", description);
}

GLFWwindow* init_window(void)
{
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
    
    GLFWwindow* window = glfwCreateWindow(1024, 768, window_title, NULL, NULL);
    if (!window)
        die("Could not create window");
    
    glfwMakeContextCurrent(window);
    //set vsync
    glfwSwapInterval(!PROFILE);
    
    //glClearColor(1.f, 0.f, 1.f, 1.f);
    //glCullFace(GL_BACK);
    //glDepthFunc(GL_LESS);
    
    return window;
}

static int xpos, ypos, width, height;

void toggle_fullscreen(GLFWwindow* window)
{
    if (glfwGetWindowMonitor(window)) {
        glfwSetWindowMonitor(window, NULL, xpos, ypos, width, height, 0);
        glfwSetWindowTitle(window, window_title);
    }
    else {
        glfwGetWindowPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &width, &height);
        
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
}
