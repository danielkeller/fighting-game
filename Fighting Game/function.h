//
//  function.h
//  Fighting Game
//
//  Created by Daniel Keller on 4/17/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#ifndef function_h
#define function_h

void* malloc(unsigned long);

typedef struct bound {
    int (*fun)(void*);
    void* arg;
} bound_t;

#define BINDABLE_PROTO(func, argty) \
    int func##bindhelper(void* arg); \
    bound_t bind_##func(argty* arg);

#define BINDABLE(func, argty) \
    int func##_bindhelper(void* arg) { \
        return func((argty*)arg); \
    } \
    bound_t bind_##func(argty* arg) { \
        argty* data = malloc(sizeof(argty)); \
        *data = *arg; \
        return (bound_t) {func##_bindhelper, data}; \
    } \
    bound_t ref_bind_##func(argty* arg) { \
        return (bound_t) {func##_bindhelper, arg}; \
    }

#define CALL_BOUND(bound) (bound).fun((bound).arg)
void free_bound(bound_t* bound);

#endif /* function_h */
