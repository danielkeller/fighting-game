//
//  input.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/25/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "input.h"
#include "window.h"

key_events_t key_left, key_right, joy_left_prev, joy_right_prev;

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

int joy_left = -1, joy_right = -1;

void joystick_callback(int joy, int event)
{
    if (event == GLFW_CONNECTED)
    {
        if (joy_left == -1) joy_left = joy;
        if (joy_right == -1) joy_right = joy;
    }
    else if (event == GLFW_DISCONNECTED)
    {
        if (joy_left == joy) joy_left = -1;
        if (joy_right == joy) joy_right = -1;
    }
}

void init_input(GLFWwindow* window)
{
    glfwSetKeyCallback(window, key_callback);
    glfwSetJoystickCallback(joystick_callback);
    
    for (int j = 0; j <= GLFW_JOYSTICK_LAST; ++j)
    {
        if (glfwJoystickPresent(j))
        {
            if (joy_left != -1)
            {
                joy_right = j;
                return;
            }
            joy_left = j;
        }
    }
}

void poll_input()
{
    glfwPollEvents();
    int count;
    if (joy_left != -1) {
        const unsigned char* axes = glfwGetJoystickButtons(joy_left,  &count);
        if (joy_left_prev.dodge != axes[1]) key_left.dodge += axes[1];
        if (joy_left_prev.attack != axes[2]) key_left.attack += axes[2];
        joy_left_prev.dodge = axes[1];
        joy_left_prev.attack = axes[2];
    }
    if (joy_right != -1) {
        const unsigned char* axes = glfwGetJoystickButtons(joy_right,  &count);
        if (joy_right_prev.dodge != axes[1]) key_right.dodge += axes[1];
        if (joy_right_prev.attack != axes[2]) key_right.attack += axes[2];
        joy_right_prev.dodge = axes[1];
        joy_right_prev.attack = axes[2];
    }
}
