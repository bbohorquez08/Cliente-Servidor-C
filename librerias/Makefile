all: sendwrite.o split.o protocol.o crear_directorio_files.o

sendwrite.o: sendwrite.c
	gcc -c -o sendwrite.o sendwrite.c

split.o: split.c
	gcc -c -o split.o split.c

protocol.o: protocol.c
	gcc -c -o protocol.o protocol.c

crear_directorio_files.o: crear_directorio_files.c
	gcc -c -o crear_directorio_files.o crear_directorio_files.c

clean:
	rm -f *.o