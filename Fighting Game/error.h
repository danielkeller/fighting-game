//
//  error.h
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef error_h
#define error_h
#include <stddef.h>

_Noreturn void die(const char* reason);
void printStackTrace(void);

#endif /* error_h */
