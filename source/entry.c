/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "entry.h"
#include "data.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados passados.
 */
struct entry_t *entry_create(char *key, struct data_t *data){

    if(data == NULL || key == NULL){
        perror("A struct data_t ou a key é NULL");
        return NULL;
    }

    struct entry_t *p;

    p = (struct entry_t*) malloc(sizeof(struct entry_t));

    if(p == NULL){
        perror("Erro na criação de uma entry");
        return NULL;
    }

    p->key = key; 
    p->value = data; 
    
    return p;
}

/* Função que inicializa os elementos de uma entrada na tabela com o
 * valor NULL.
 */
void entry_initialize(struct entry_t *entry){
    
    entry->key = NULL;
    entry->value = NULL;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry){
    
    if(entry != NULL){
        if(entry->key != NULL){
            free(entry->key);
        }
        if(entry->value != NULL){
            if(entry->value->data != NULL){
                free(entry->value->data);
            }
            free(entry->value);
        }
        free(entry);
    }
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry){

    if(entry == NULL){
        perror("A entry é NULL");
        return NULL;
    }

    struct entry_t *p;

    p = (struct entry_t *) malloc(sizeof(struct entry_t));

    if(p == NULL){
        perror("Erro na criação da entry");
        return NULL;
    }

    p->key = malloc(sizeof(entry->key));
    memcpy(p->key, entry->key, strlen(entry->key) + 1);
    p->value = data_dup(entry->value);
    
    return p;
}

/* Função que substitui o conteúdo de uma entrada entry_t.
*  Deve assegurar que destroi o conteúdo antigo da mesma.
*/
void entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
    
    entry_destroy(entry);
    entry = entry_create(new_key, new_value);
}

/* Função que compara duas entradas e retorna a ordem das mesmas.
*  Ordem das entradas é definida pela ordem das suas chaves.
*  A função devolve 0 se forem iguais, -1 se entry1<entry2, e 1 caso contrário.
*/
int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
    
    char* key1;
    char* key2;
    int resultado;
    
    key1 = entry1->key;
    key2 = entry2->key;

    resultado = strcmp(key1, key2);
    if(resultado == 0){
        return 0;
    }
    else if(resultado < 0){
        return -1;
    }
    else{
        return 1;
    }
}


