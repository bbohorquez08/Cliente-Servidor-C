all: client.o
	gcc -o client client.o ../librerias/protocol.o ../librerias/split.o ../librerias/sendwrite.o ../librerias/crear_directorio_files.o
client.o: client.c
	gcc -c -I../librerias -o client.o client.c
clean:
	rm -f client *.o