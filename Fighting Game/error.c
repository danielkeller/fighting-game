//
//  error.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

void die(const char* reason)
{
    printf("%s\n", reason);
    exit(1);
}

void* malloc_or_die(size_t size)
{
    void* ret = malloc(size);
    if (!ret) die("out of memory");
    return ret;
}

void printStackTrace(void)
{
    static const unsigned int max_frames = 63;
    // storage array for stack trace address data
    void* addrlist[max_frames+1];
    
    // retrieve current stack addresses
    unsigned int addrlen = backtrace(addrlist, (int)sizeof( addrlist ) / sizeof( void* ));
    char** symbollist = backtrace_symbols( addrlist, addrlen );
    backtrace_symbols_fd(addrlist, addrlen, 2);
    
    free(symbollist);
}
