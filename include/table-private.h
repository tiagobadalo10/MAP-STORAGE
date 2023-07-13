#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "list.h"

struct table_t {
    struct list_t **lists;
    int numberLists;
};


/* Função que transforma uma chave num indice da tabela
 *
 */
int hash(char* key, int n);

/* Função que soma os valores ASCII dos caracteres de uma string
 *
 */
int soma(char* string);


#endif
