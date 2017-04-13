//
//  file.h
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef file_h
#define file_h
#include <stdint.h>

#ifdef __APPLE__
typedef int watch_t;
#endif

watch_t make_watch();
void free_watch(watch_t);
void watch_file(watch_t watch, const char* name, void* udata);
void* poll_watch(watch_t);

char* read_file(const char* name, size_t* length);

#endif /* file_h */
