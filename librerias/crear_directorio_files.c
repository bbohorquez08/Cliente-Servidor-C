#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
int crear_directorio_files()
{
    struct stat s;

    //Verifica si el directorio existe
    if(stat("./files/", &s) == 0)
    {
        if (S_ISDIR(s.st_mode))
            return 1;
        else
            return 0;
    }
    //Verifica si se puede crear el directorio
    if(stat("./files/", &s) != 0)
    {
        if (mkdir("./files/", 0755) != 0)
        {
            fprintf(stderr, "No se puede crear el directorio de files");
            return 0;
        }    
        else
            return 1;
    }
}