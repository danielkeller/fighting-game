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
    update_button(&to->move, &from->move);
    update_button(&to->special, &from->special);
}

struct key_events key_left, key_right;

//State should be pressed=1 or released=0
void binding(struct button* button, int state)
{
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
    
    if (key == GLFW_KEY_C)      binding(&key_left.move, action);
    if (key == GLFW_KEY_SLASH)  binding(&key_right.move, action);
    if (key == GLFW_KEY_X)      binding(&key_left.attack, action);
    if (key == GLFW_KEY_PERIOD) binding(&key_right.attack, action);
    if (key == GLFW_KEY_Z)      binding(&key_left.dodge, action);
    if (key == GLFW_KEY_COMMA)  binding(&key_right.dodge, action);
    if (key == GLFW_KEY_S)      binding(&key_left.special, action);
    if (key == GLFW_KEY_L)      binding(&key_right.special, action);
    
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
    
    for (int j = 0; j <= GLFW_JOYSTICK_LAST; ++j) {
        if (glfwJoystickPresent(j)) {
            if (joy_left != -1) {
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
        //Button-finding code
        /*
        for (int i = 0; i < count; ++i)
            if (axes[i])
                printf("%d ", i);
        printf("\n");
        */
        
                 key_left.start = axes[9];
        binding(&key_left.move,   axes[7]);
        binding(&key_left.dodge,  axes[1]);
        binding(&key_left.attack, axes[2]);
    }
    if (joy_right != -1) {
        const unsigned char* axes = glfwGetJoystickButtons(joy_right,  &count);
                 key_right.start = axes[9];
        binding(&key_right.move,   axes[7]);
        binding(&key_right.dodge,  axes[1]);
        binding(&key_right.attack, axes[2]);
    }
}
