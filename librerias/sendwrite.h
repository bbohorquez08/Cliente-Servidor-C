#ifndef SENDWRITE_H
#define SENDWRITE_H

int send_to_socket(int sd, int fd, unsigned int filesize);
void write_from_socket(int sd, int fd, unsigned int filesize);

#endif