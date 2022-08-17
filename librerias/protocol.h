#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <limits.h>

//#define REQ_GET 1
//#define REQ_PUT 2
//#define REQ_EXIT 3

typedef struct{
    char operation[16];
    char filename[PATH_MAX];
    unsigned int size; //CUANDO EL TAMANIO ES -1 EL ARCHIVO NO EXISTE
    mode_t mode; // PERMISOS PARA EL ARCHIVO ¡IMPORTANTE!
}mensaje;

typedef struct{
    char filename[PATH_MAX];
    int error;
}file_info;

struct sockaddr_in * server_address(unsigned short port);
struct sockaddr_in * address_by_ip(char * ip, unsigned short port);
struct sockaddr_in * address_by_hostname(char * hostname, unsigned short port);

#define EQUALS(s1,s2) (strcmp(s1,s2) == 0)
#endif