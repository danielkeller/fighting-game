//
//  file.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "file.h"
#include "error.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

char* read_file(const char* name, size_t* length)
{
    struct stat sb;
    if (stat(name, &sb))
        die(strerror(errno));
    
    char* ret = malloc_or_die(sb.st_size);
    
    FILE* file = fopen(name, "rb");
    if (!file)
        die(strerror(errno));
    
    if (fread(ret, 1, sb.st_size, file) != sb.st_size)
        die("fread");
    
    if (fclose(file))
        die("fclose");
    
    *length = sb.st_size;
    return ret;
}