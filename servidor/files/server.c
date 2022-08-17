#include <arpa/inet.h>
#include <netinet/in.h> //IPV4
#include <pthread.h> //HILOS
#include <signal.h>
#include <semaphore.h> //SEMAFOROS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>


int finished;

#include "protocol.h"
#include "split.h"
#include "sendwrite.h"
#include "crear_directorio_files.h"

#define MAX_CLIENTES 100
#define MAX_ARCHIVOS 50

sem_t sem_clients;
sem_t mutex_clients;
sem_t arr_mutex;

//ESTRUCTURA QUE ME PERMITE CONTROLOAR EL ESTADO DEL ARCHIVO
typedef struct {
    char filename[PATH_MAX];
    sem_t mutex_archivo;
}estado_archivo;

//ARRAY ESTADO DE ARCHIVOS
typedef struct {
    estado_archivo *array_archivo[MAX_ARCHIVOS];
    int size;
}array_estado_archivo;

int array_cliente[MAX_CLIENTES];
array_estado_archivo *array_fs;

void * controlador_conexion(void *);
void controlador_siguiente(int argc);
int cliente_siguiente();
int archivo_vacio();
void cerrar_cliente(int);
void cerrar_servidor();
estado_archivo * obtener_estado(char *);

void inicializarServidor(int prmPuerto);
void get(mensaje *prmHeader,int prmClient_Socket,int fd,char *direccion,struct stat stats);
void put(mensaje *prmHeader,int prmClient_Socket,int fd,char *direccion);


int main(int argc, char *argv[]){

    if (signal(SIGINT, controlador_siguiente) == SIG_ERR){
        printf("No es posible manejar la señal SIGINT\n");
    }
    if (argc != 2){
        fprintf(stderr, "Debe especificar el puerto a escuchar\n");
        exit(EXIT_FAILURE);
    }

    inicializarServidor(atoi(argv[1]));
}

void inicializarServidor(int prmPuerto){
    //SI NO EXISTE, SE CREA EL DIRECTORIO FILES
    crear_directorio_files();
    //SOCKET DEL SERVIDOR
    int s;
    //SOCKET DEL CLIENTE
    int c;
    //DIRECCION DEL SERVIDOR (IPV4)
    struct sockaddr_in * addr;
    //SOCKET IPV4, DE TIPO FLUJO (STREAM)
    //1. CREAR UN SOCKET
    s=socket(PF_INET,SOCK_STREAM,0);
    if (s < 0){
        printf("No es posible crear el socket\n");
        exit(EXIT_FAILURE);
    }

    //Obtener una direccion y un puerto 
    addr = server_address(prmPuerto);

    //2. ASOCIAR EL SOCKET A UNA DIRECCION IPV4
    if(bind(s,(struct sockaddr *)addr,sizeof(struct sockaddr_in))<0){
        printf("Operacion BIND fallida\n");
        exit(EXIT_FAILURE);
    }

    //3. PONER EL SOCKET LISTO
    if(listen(s,10)){
        printf("No es posible escuchar el socket\n");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchado por el puerto: %d\n", prmPuerto);

    sem_init(&sem_clients, 0, MAX_CLIENTES);
    sem_init(&mutex_clients, 0, 1);
    sem_init(&arr_mutex, 0, 1);

    memset(array_cliente,0,MAX_CLIENTES);
    array_fs=malloc(sizeof(array_estado_archivo));
    array_fs->size=0;
    finished=0;
    while (!finished){
        //4.ACEPTAR LA CONEXION
        printf("Waiting for conections...\n");
        c= accept(s,0,0);
        if (c < 0){
            printf("No es posible aceptar al cliente\n");
            continue;
        }
        printf("Client %d connected!\n",c);

        pthread_t pt;
        int *clienteP=malloc(sizeof(int));
        *clienteP=c;

        int indice=cliente_siguiente();

        sem_wait(&sem_clients);
        sem_wait(&mutex_clients);
        array_cliente[indice]=c;
        sem_post(&mutex_clients);

        pthread_create(&pt,NULL,controlador_conexion,clienteP);
    } 
}

int cliente_siguiente(){
    for (int i = 0; i < MAX_CLIENTES; i++){
        if (array_cliente[i] == 0){
            return i;
        }
    }
    return -1;
}

void cerrar_cliente(int argc){
    for (int i = 0; i < MAX_CLIENTES; i++){
        if (argc == array_cliente[i])
        {
            close(argc);
            array_cliente[i] = 0;
            break;
        }
    }
    sem_post(&sem_clients);
}

void cerrar_servidor(){
    int i = 0;
    sem_wait(&mutex_clients);

    while (array_cliente[i] != 0 && i < MAX_CLIENTES){
        printf("cerrando la conexion con el cliente %d\n",array_cliente[i]);
        cerrar_cliente(array_cliente[i]);
        i++;
    }

    sem_post(&mutex_clients);
}

estado_archivo * obtener_estado(char * filename){
    estado_archivo *fs = 0;

    sem_wait(&arr_mutex);

    for (int i = 0; i < array_fs->size; i++){
        if (strcmp(filename, array_fs->array_archivo[i]->filename) == 0){
            fs = array_fs->array_archivo[i];
        }
    }

    if (array_fs->size < MAX_ARCHIVOS && fs == 0 && filename != 0){
        fs = malloc(sizeof(estado_archivo));
        sem_init(&fs->mutex_archivo, 0, 1);
        strcpy(fs->filename, filename);
        array_fs->array_archivo[array_fs->size++] = fs;
    }

    sem_post(&arr_mutex);
    return fs;
}

void * controlador_conexion(void * c){
    struct stat stats;
    int fd;

    char direccion[PATH_MAX];
    mensaje *header;

    int nread;
    int client_s=*(int *)c;

    free(c);

    while(1){
        //Recibe una peticion de enviar o recibir un archivo
        header = malloc(sizeof(mensaje));
        nread = read(client_s, header, sizeof(mensaje));
        //VALIDACION
        if (nread <= 0){
            break;
        }

        printf("Leido del proceso cliente %d (%d bytes): %s %s\n", client_s, nread, header->operation, header->filename);
        fd=0;
        memset(direccion,0,PATH_MAX);

        //DIRECCION
        strcpy(direccion, "./files/");
        strcat(direccion, header->filename);

        estado_archivo *fs = obtener_estado(header->filename);
        if (fs == 0){
            continue;
        }

        sem_wait(&fs->mutex_archivo);
        if(strcmp(header->operation, "put") == 0){
            put(header,client_s,fd,direccion);
        }else if (strcmp(header->operation, "get") == 0){
            get(header,client_s,fd,direccion,stats);
        }else{
            printf("No es posible reconocer el comando\n");
        }
        sem_post(&fs->mutex_archivo);
    }

    printf("Conexion con el cliente %d terminada\n",client_s);

    /* Cerrar la conexion */
    sem_wait(&mutex_clients);
    cerrar_cliente(client_s);
    sem_post(&mutex_clients);

    return 0; 
}
void put(mensaje *prmHeader,int prmClient_Socket,int fd,char *direccion){
        printf("Archivo recibido %d, guardando en: %s\n", prmHeader->size,direccion);
        fd = open(direccion, O_RDWR | O_CREAT | O_TRUNC, prmHeader->mode);
        if (fd < 0){
            printf("No es posible guardar el archivo\n");
            return;
        }

            write_from_socket(prmClient_Socket, fd, prmHeader->size);
            
            close(fd);
            printf("El archivo fue guardado exitosamente\n");
}
void get(mensaje *prmHeader,int prmClient_Socket,int fd,char *direccion,struct stat stats){
       //Se verifica la existencia del archivo
            if (stat(direccion, &stats) != 0){
                printf("No se pudo encontrar el archivo\n");
                printf("Compruebe la ruta y/o nombre del archivo: %s\n", direccion);
                
            }
            prmHeader->size = stats.st_size;
            prmHeader->mode = stats.st_mode;

            send(prmClient_Socket,prmHeader, sizeof(mensaje), 0);

            if (stats.st_size <= 0){
                printf("No se puede enviar un archivo que se encuentre vacio.\n");
                return;
            }

            fd = open(direccion, O_RDONLY);

            //COMPROBAR EL ACCESO AL ARCHIVO
            if (fd <= 0)
            {
                return;
            }

            printf("Enviando archivo de tamaño %d: %s\n", prmHeader->size, prmHeader->filename);

            send_to_socket(prmClient_Socket, fd, prmHeader->size);
            close(fd);
            printf("Archivo enviado al cliente: %d\n", prmClient_Socket);
}

void controlador_siguiente(int argc){
    if (argc == SIGINT){
        printf("\nTerminando el proceso del servidor\n");
        cerrar_servidor();
        exit(EXIT_SUCCESS);
    }
}