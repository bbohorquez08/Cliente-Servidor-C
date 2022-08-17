#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/socket.h>

#include "sendwrite.h"

int send_to_socket(int sd, int fd, unsigned int filesize){
    char buffer[BUFSIZ + 1];
    int rest;
    int nread;
    int buffersize;
    rest = filesize;

    while (rest > 0){
        buffersize = BUFSIZ;
        if (rest < BUFSIZ){
            buffersize = rest;
        }
        memset(buffer, 0, BUFSIZ + 1);
        nread = read(fd, buffer, buffersize);
        if (nread < 0 && errno == EINTR){
            continue;
        }
        if (nread <= 0){
            return -1;
        }
        send(sd, buffer, nread, 0);
        rest = rest - nread;
    }
    return 1;
}

void write_from_socket(int sd, int fd, unsigned int filesize){
    char buffer[BUFSIZ + 1];
    int rest;
    int nread;
    int buffersize;

    rest = filesize;

    while (rest > 0){
        buffersize = BUFSIZ;

        if (rest < BUFSIZ){
            buffersize = rest;
        }
        memset(buffer, 0, BUFSIZ + 1);
        nread = read(sd, buffer, buffersize);
        if (nread < 0 && errno == EINTR){
            continue;
        }
        if (nread <= 0){
            break;
        }
        write(fd, buffer, nread);
        rest = rest - nread;
    }
}