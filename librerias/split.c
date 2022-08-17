/* 
 * String split library
 * Author Erwin Meza Vega <emezav@gmail.com> 
 * Facilita la divisi�n de una cadena de caracteres en "palabras"
 * que se encuentran rodeadas de "delimitadores"
*/
#include "split.h"
#include <string.h>
#include <stdlib.h>


/** Delimitadores por defecto */
const char * split_default_delimiters = " \t\r\n";

/** Retorna los delimitadores por defecto */
const char * default_delimiters();

/** Retorna un apuntador a la posici�n de la cadena en la cual se encuentra 
 * el primer caracter que no es delimitador
 */
char * skip_delimiters(char * str, const char * delim);

/**
 * Retorna un apuntador a la posici�n de la cadena en la cual se encuentra un 
 * caracter delimitador, o el fin de la cadena (nulo) 
 */
char * skip_non_delimiters(char * str, const char * delim);

/** 
 * Divide una cadena en palabras, usando los delimitadores especificados
 * o los delimitadores por defecto
 */
split_list * split(char * str, const char * delim) {
    char * str_ptr;
    char * pos;
    int i;
    int count;

    split_list * ret;
    
    ret = (split_list *) malloc(sizeof(split_list));

    ret->count = 0;
    for (i=0; i< MAX_PARTS; i++) {
    	ret->parts[i] = 0;	
    }

    i = 0;
    str_ptr = skip_delimiters(str, delim);
    while (str_ptr != 0 && *str_ptr != 0) {
	      pos = skip_non_delimiters(str_ptr, delim);

      	if (pos == 0) {
      	    break;
      	}
      
      	count = pos - str_ptr;
      
      	ret->parts[i] = (char*)malloc(count + 1);
      	memset(ret->parts[i], 0, count + 1);
      	strncpy(ret->parts[i], str_ptr, count);
      	i++;
      	str_ptr = skip_delimiters(pos, delim);
    }

    ret->count = i;

    return ret;
}

/** Retorna un apuntador a la posici�n de la cadena en la cual se encuentra 
 * el primer caracter que no es delimitador
 */
char * skip_delimiters(char * str, const char * delim) {
    char * str_ptr;
    const char * delim_ptr;

    if (str == 0) {
	   return 0;
    }

    delim_ptr = delim;
    if (delim_ptr == 0 || strlen(delim_ptr) == 0) {
    	delim_ptr = default_delimiters();
    }

    str_ptr = str;

    for (str_ptr = str; *str_ptr != 0 && strchr(delim_ptr, *str_ptr) != 0; str_ptr++);

    return str_ptr;

}

/**
 * Retorna un apuntador a la posici�n de la cadena en la cual se encuentra un 
 * caracter delimitador, o el fin de la cadena (nulo) 
 */
char * skip_non_delimiters(char * str, const char * delim) {
    char * str_ptr;
    const char * delim_ptr;

    if (str == 0) {
	     return 0;
    }

    delim_ptr = delim;

    if (delim_ptr == 0 || strlen(delim_ptr) == 0) {
    	delim_ptr = default_delimiters();
    }

    str_ptr = str;

    for (str_ptr = str; *str_ptr != 0 && strchr(delim_ptr, *str_ptr) == 0; str_ptr++);

    return str_ptr;

}

/** Retorna los delimitadores por defecto */
const char * default_delimiters() {
    return split_default_delimiters;
}
