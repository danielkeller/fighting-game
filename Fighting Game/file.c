//
//  file.c
//  Fighting Game
//
//  Created by Daniel Keller on 8/18/16.
//  Copyright © 2016 Daniel Keller. All rights reserved.
//

#include "file.h"
#include "error.h"

#include <string.h>
#include <errno.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/syslimits.h>

void rewatch_deleted(watch_t kq, int fd, void* udata);

watch_t make_watch()
{
    int fd = kqueue();
    if (fd == -1)
        die(strerror(errno));
    return fd;
}

void free_watch(watch_t kq)
{
    close(kq);
}

void* poll_watch(watch_t kq)
{
    struct kevent ev;
    struct timespec ts = {0}; //Poll
    int ret = kevent(kq, NULL, 0, &ev, 1, &ts);
    
    if (ret == -1)
        die(strerror(errno));
    else if (ret == 0)
        return NULL;
    else
    {
        if (ev.fflags & NOTE_DELETE) //Atomic save
            rewatch_deleted(kq, (int)ev.ident, ev.udata);
        return ev.udata;
    }
}

void watch_file(watch_t kq, const char* name, void* udata)
{
    int fd = open(name, O_EVTONLY);
    if (fd == -1)
        die(strerror(errno));
    
    struct kevent change;
    //use NOTE_DELETE to watch for atomic saves; a new file is written and moved on top of the old one
    EV_SET(&change, fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_WRITE | NOTE_DELETE, 0, udata);
    
    if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
        die(strerror(errno));
}

//Handle the case that a file was deleted. We'll need a new fd.
void rewatch_deleted(watch_t kq, int fd, void* udata)
{
    char name[PATH_MAX];
    if (fcntl(fd, F_GETPATH, name) == -1)
        die(strerror(errno));
    
    //Will automatically remove the event from kq
    close(fd);
    
    watch_file(kq, name, udata);
}

char* read_file(const char* name)
{
    struct stat sb;
    if (stat(name, &sb))
        die(strerror(errno));
    
    char* ret = malloc_or_die(sb.st_size + 1);
    
    int fd = open(name, O_RDONLY);
    if (fd == -1)
        die(strerror(errno));
    
    if (read(fd, ret, sb.st_size) != sb.st_size)
        die("read");
    
    close(fd);
    
    ret[sb.st_size] = 0;
    return ret;
}
#endif
