//
//  window.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/4/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef window_h
#define window_h

//Window stuff
#define GLFW_INCLUDE_NONE
#include "GLFW/include/GLFW/glfw3.h"

typedef struct GLFWwindow GLFWwindow;
GLFWwindow* init_window();

#endif /* window_h */
