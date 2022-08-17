/* 
 * String split library
 * Author Erwin Meza Vega <emezav@gmail.com> 
*/

#ifndef SPLIT_H_
#define SPLIT_H_

#define MAX_PARTS 255

/**
 * Define la estructura de la lista de palabras obtenida como valor de retorno
 * de la funci�n split.
 */
typedef struct {
    char * parts[MAX_PARTS];
    int count;
}split_list;

/** 
 * Divide una cadena en palabras, usando los delimitadores especificados
 * o los delimitadores por defecto
 */
split_list * split(char * str, const char * delim);

#endif
