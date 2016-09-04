//
//  math.h
//  Fighting Game
//
//  Created by Daniel Keller on 9/3/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#ifndef math_h
#define math_h

typedef struct Mat3 {
    float d[9];
} Mat3;

extern Mat3 eye3;

Mat3 affine(float theta, float x, float y);

#endif /* math_h */
