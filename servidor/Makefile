all: server.o
	gcc -pthread -lrt -o server server.o ../librerias/protocol.o ../librerias/split.o ../librerias/sendwrite.o ../librerias/crear_directorio_files.o
server.o: server.c
	gcc -c -I../librerias -o server.o server.c
clean:
	rm -f server *.o