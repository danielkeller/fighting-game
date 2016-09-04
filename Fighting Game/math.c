//
//  math.c
//  Fighting Game
//
//  Created by Daniel Keller on 9/3/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "math.h"
#include <math.h>

Mat3 eye3 = {{
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
}};

Mat3 affine(float theta, float x, float y)
{
    if (theta == 0.)
        return (Mat3){{
            1, 0, 0,
            0, 1, 0,
            x, y, 1
        }};
        
    return (Mat3){{
        cosf(theta),  sinf(theta), 0,
        -sinf(theta), cosf(theta), 0,
        x,            y,           1
    }};
}
