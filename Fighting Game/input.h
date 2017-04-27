//
//  input.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/25/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef input_h
#define input_h

struct GLFWwindow;
void init_input(struct GLFWwindow* window);
void poll_input();

typedef struct key_events {
    int attack, dodge, start;
} key_events_t;

extern key_events_t key_left, key_right;

#endif /* input_h */
