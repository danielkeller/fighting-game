//
//  input.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/25/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "engine.h"
#include "window.h"

int shift_button_press(struct button* b)
{
    int ret = b->pressed != 0;
    b->pressed = 0;
    return ret;
}

int shift_button_cancel(struct button* b)
{
    int ret = b->cancelled != 0;
    b->cancelled = 0;
    return ret;
}

void update_button(struct button* to, struct button* from)
{
    to->down = from->down;
    to->pressed += from->pressed;
    to->cancelled |= from->cancelled;
    from->pressed = 0;
    from->cancelled = 0;
}

void update_key_events(struct key_events* to, struct key_events* from)
{
    update_button(&to->attack, &from->attack);
    update_button(&to->dodge, &from->dodge);
    update_button(&to->fwd, &from->fwd);
    update_button(&to->back, &from->back);
    update_button(&to->special, &from->special);
}

struct key_events key_left, key_right;

//State should be pressed=1 or released=0
void binding(struct button* button, int state)
{
    //Ignore repeat events from joystick
    if (button->down == state)
        return;
    
    if (state && !button->down) {
        ++button->pressed;
        button->press_time = game_time.frame;
        button->cancelled = 0;
    }
    if (!state && game_time.frame - button->press_time <= TAP_FRAMES)
        button->cancelled = 1;
    button->down = state;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT) return;
    
    if (key == GLFW_KEY_V)           binding(&key_left.back, action);
    if (key == GLFW_KEY_C)           binding(&key_left.fwd, action);
    if (key == GLFW_KEY_RIGHT_SHIFT) binding(&key_right.back, action);
    if (key == GLFW_KEY_SLASH)       binding(&key_right.fwd, action);
    if (key == GLFW_KEY_X)           binding(&key_left.attack, action);
    if (key == GLFW_KEY_PERIOD)      binding(&key_right.attack, action);
    if (key == GLFW_KEY_Z)           binding(&key_left.dodge, action);
    if (key == GLFW_KEY_COMMA)       binding(&key_right.dodge, action);
    if (key == GLFW_KEY_S)           binding(&key_left.special, action);
    if (key == GLFW_KEY_L)           binding(&key_right.special, action);
    
    if (action == GLFW_RELEASE) return;
    if (key == GLFW_KEY_F) toggle_fullscreen(window);
    if (key == GLFW_KEY_P) learning_mode = !learning_mode;
}

int joy_left = -1, joy_right = -1;

void joystick_callback(int joy, int event)
{
    if (event == GLFW_CONNECTED) {
        if (joy_left == -1) joy_left = joy;
        else if (joy_right == -1) joy_right = joy;
    }
    else if (event == GLFW_DISCONNECTED) {
        if (joy_left == joy) joy_left = -1;
        else if (joy_right == joy) joy_right = -1;
    }
}

void init_input(GLFWwindow* window)
{
    glfwSetKeyCallback(window, key_callback);
    glfwSetJoystickCallback(joystick_callback);
    
    for (int j = 0; j <= GLFW_JOYSTICK_LAST; ++j)
        if (glfwJoystickPresent(j))
            joystick_callback(j, GLFW_CONNECTED);
}

static const float axis_button_threshold = 1./3.;

void poll_input()
{
    glfwPollEvents();
    int count;
    if (joy_left != -1) {
        const unsigned char* buttons = glfwGetJoystickButtons(joy_left,  &count);
        const float* axes = glfwGetJoystickAxes(joy_left, &count);
        //Button-finding code
        /*
        for (int i = 0; i < count; ++i)
            if (buttons[i])
                printf("%d ", i);
        printf("\n");
         */
        
        //Axis-finding code
        /*
         for (int i = 0; i < count; ++i)
         printf("%f ", axes[i]);
         printf("\n");
         */
        
                 key_left.start =  buttons[9];
        binding(&key_left.fwd,     axes[0] >  axis_button_threshold);
        binding(&key_left.back,    axes[0] < -axis_button_threshold);
        binding(&key_left.dodge,   buttons[1]);
        binding(&key_left.attack,  buttons[0]);
        binding(&key_left.special, buttons[3]);
    }
    if (joy_right != -1) {
        const unsigned char* buttons = glfwGetJoystickButtons(joy_right,  &count);
        const float* axes = glfwGetJoystickAxes(joy_right, &count);
        
                 key_right.start =  buttons[9];
        binding(&key_right.fwd,     axes[0] < -axis_button_threshold);
        binding(&key_right.back,    axes[0] >  axis_button_threshold);
        binding(&key_right.dodge,   buttons[1]);
        binding(&key_right.attack,  buttons[0]);
        binding(&key_right.special, buttons[3]);
    }
}
