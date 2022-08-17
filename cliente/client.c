#include <arpa/inet.h> //inet_aton, inet_ntoa, ....
#include <netinet/in.h> //IPV4
#include <pthread.h> //HILOS
#include <semaphore.h> //SEMAFOROS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "protocol.h"
#include "split.h"
#include "sendwrite.h"
#include "crear_directorio_files.h"

void put(mensaje *prmHeader,int prmSocket,int fd,split_list *l,char *direccion,struct stat stats);
void get(mensaje *prmHeader,int prmSocket,int nread,int fd,split_list *l,char *direccion);
void ayuda();

int main(int argc, char *argv[]){
    //SI NO EXISTE, SE CREA EL DIRECTORIO FILES
    crear_directorio_files();
    //SOCKET DEL SERVIDOR
    int s;
    char comando[BUFSIZ];
    //DIRECCION DEL SERVIDOR (IPV4)
    struct sockaddr_in *addr;
    //DIRECCION DE CARPETA ARCHIVOS
    char direccion[PATH_MAX];
    split_list *l;
    //ARCHIVOS
    int fd;
    //PROTOCOLO A UTILIZAR
    mensaje *header;
    struct stat stats;
    //CONTADOR
    int nread;    
    //VALIDACION: se obtiene la direccion IP del host a conectar 
    if(argc<3){
        printf("\nFaltan parametros.\nDebe indicar la ip del servidor y el puerto\n");
        exit(EXIT_FAILURE);
    }
    
    //SOCKET IPV4, DE TIPO FLUJO (STREAM)
    //1. CREAR UN SOCKET
    s=socket(AF_INET,SOCK_STREAM,0);
    if (s < 0){
        printf("No es posible la creacion del socket\n");
        exit(EXIT_FAILURE);
    }
    //PREPARAR LA DIRECCION Y ASOCIARLA AL SOCKET
    memset(&addr,0,sizeof(struct sockaddr_in));
    addr=address_by_ip(argv[1], atoi(argv[2]));
    printf("Connecting...\n");

    
    //2. VALIDACION: CONECTARSE AL SERVIDOR
    if(connect(s,(struct sockaddr *)addr,sizeof(struct sockaddr_in))!=0){
        printf("No es posible hacer conexion al servidor\n");
        exit(EXIT_FAILURE);
    }
    printf("Connected!\n");
    
    
    while (1){
        //LIMPIAMOS BASURA (LLENAMOS DE 0)
        memset(comando,0,BUFSIZ);
        //LEEMOS COMANDO
        printf("Ingrese el comando. (help=ayuda)\n");
        printf("$ ");
        fgets(comando, BUFSIZ, stdin);
        l=split(comando, " \n\r\t");

        if(l->count == 0){
            continue;
        }

        fd=0;
        //LIMPIAMOS BASURA (LLENAMOS DE 0)
        memset(direccion,0,PATH_MAX);
        if(strcmp(l->parts[0],"help") == 0 || strcmp(l->parts[0],"ayuda") == 0){
            ayuda();
            continue;
        }
        if (strcmp(l->parts[0], "exit") == 0){
            break;
        }else if (l->count != 2){
            continue;
        }

        
        //FICHERO (ARCHIVOS)
        strcpy(direccion, "./files/");
        strcat(direccion, l->parts[1]);
        //MENSAJE A ENVIAR AL SERVIDOR
        header = malloc(sizeof(mensaje));

        if (strcmp(l->parts[0], "put") == 0||strcmp(l->parts[0], "Put") == 0||strcmp(l->parts[0], "PUT") == 0){
            put(header,s,fd,l,direccion,stats);
        }else if (strcmp(l->parts[0], "get") == 0||strcmp(l->parts[0], "Get") == 0||strcmp(l->parts[0], "GET") == 0){
            get(header,s,nread,fd,l,direccion);
        }else{
            printf("Comando desconocido!\n");
            continue; 
        }
    }   
    printf("Conexion Terminada.\n");
    close(s);
    exit(EXIT_SUCCESS);

}

void ayuda(){
    printf("Comandos disponibles:\n");
    printf("get ARCHIVO: Transfiere un archivo desde el servidor hacia el cliente.\n");
    printf("put ARCHIVO: Transfiere un archivo desde el cliente hacia el servidor.\n");
    printf("exit: Cerrar la conexión.\n");
}

void get(mensaje *prmHeader,int prmSocket,int nread,int fd,split_list *l,char *direccion){
            //HEADER PARA ENVIAR AL SERVIDOR
            strcpy(prmHeader->operation, "get");         // peticion
            strcpy(prmHeader->filename, l->parts[1]); // nombre del archivo
            //NOTIFICACION DE RECIBIR UN ARCHIVO
            send(prmSocket, prmHeader, sizeof(mensaje), 0);
            //RECIBIR TAMAÑO DEL ARCHIVO SOLICITADO
            nread = read(prmSocket, prmHeader, sizeof(mensaje));
            if (nread <= 0 || prmHeader->size <= 0){
                printf("No es posible recibir el archivo.\n[%s] No existe en el servidor o se encuentra vacio.\n",l->parts[1]);
                return;
            }
            printf("Archivo de tamaño %d recibido , guardando en: %s\n", prmHeader->size, direccion);

            fd=open(direccion,O_RDWR | O_CREAT | O_TRUNC,prmHeader->mode);
            
            //VALIDACION SI SE PUEDE CREAR O ESCRIBIR EL ARCHIVO
            if (fd < 0){
                printf("No es posible guardar el archivo\n");
                return;
            }

            //LEER POR PARTES EL CONTENIDO DEL ARCHIVO
            write_from_socket(prmSocket, fd, prmHeader->size);

            //CERRAR
            close(fd);
            printf("Archivo guardado exitosamente\n");
}

void put(mensaje *prmHeader,int prmSocket,int fd,split_list *l,char *direccion,struct stat stats){
            //SE VERIFICA LA EXISTENCIA DEL ARCHIVO
            if (stat(direccion, &stats) != 0){
                printf("El archivo no existe\n");
                printf("Verifique la ruta y/o nombre del archivo: %s\n", direccion);
                return;
            }

            //COMPRUEBA QUE EL ARCHIVO NO ESTA VACIO
            if (stats.st_size <= 0){
                printf("No es posible enviar este archivo.\nMotivo: se encuentra vacio\n");
                return;
            }

            //LLENAR ESTRUCTURA DEL PROTOCOLO
            strcpy(prmHeader->operation,"put");
            strcpy(prmHeader->filename, l->parts[1]);
            prmHeader->size=stats.st_size;
            prmHeader->mode=stats.st_mode;

            //ENVIAR AL SERVIDOR
            send(prmSocket,prmHeader,sizeof(mensaje),0);

            //ABRIR EN MODO LECTURA
            fd=open(direccion,O_RDONLY);
            //VALIDACION DE ABRIR EL ARCHIVO
            if (fd < 0){
                printf("No es posible abrir el archivo\n");
                return;
            }

            //LEER POR PARTES Y ENVIAR AL SERVIDOR
            send_to_socket(prmSocket, fd, stats.st_size);
            printf("Envio de archivo con peso: (%d) y nombre: %s\n", prmHeader->size, prmHeader->filename);

            close(fd);
            printf("Archivo enviado al servidor\n");
}


