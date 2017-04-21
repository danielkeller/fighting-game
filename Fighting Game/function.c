//
//  function.c
//  Fighting Game
//
//  Created by Daniel Keller on 4/17/17.
//  Copyright © 2017 Daniel Keller. All rights reserved.
//

#include "function.h"

void free_bound(bound_t* bound)
{
    free(bound->arg);
}
