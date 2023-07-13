/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "data.h"
#include "table.h"
#include "list-private.h"
#include "table-private.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



/* Função para criar/inicializar uma nova tabela hash, com n
 * linhas (n = módulo da função hash)
 * Em caso de erro retorna NULL.
 */
struct table_t *table_create(int n){


    struct table_t *table;
    
    table = (struct table_t *) malloc(sizeof(struct table_t));

    if(table == NULL){
        perror("Erro na criação da table");
        return NULL;
    }

    table->lists = (struct list_t **) malloc(n * sizeof(struct list_t *));


    if(table->lists == NULL){
        perror("Erro na criação das lists");
        free(table);
        return NULL;
    }
    
    int i;

    for(i = 0; i < n; i++){
        table->lists[i] = list_create();
    }    

    table->numberLists = n;

    return table;

}

/* Função para libertar toda a memória ocupada por uma tabela.
 */
void table_destroy(struct table_t *table){
    
    int i;

    if(table != NULL){
        if(table->lists != NULL){
            for(i = 0; i < table->numberLists; i++){
                if(table->lists[i] != NULL){
                    list_destroy(table->lists[i]);
                }
            }
        free(table->lists);
        }
    free(table);
    }

    
}

/* Função para adicionar um par chave-valor à tabela.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na tabela,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t *table, char *key, struct data_t *value){
    
    if(table == NULL || key == NULL || value == NULL){
        perror("A table ou a key ou a struct data_t são NULL");
        return -1;
    }

    char* keyCopy = malloc(strlen(key) + 1);


    if(keyCopy == NULL){
        perror("Erro na criação da keyCopy");
        return -1;
    }

    struct data_t *valueCopy = malloc(sizeof(struct data_t));

    if(valueCopy == NULL){
        perror("Erro na criação da struct data_t");
        return -1;
    }

    valueCopy->data = malloc(value->datasize);

    if(valueCopy->data == NULL){
        perror("Erro na criação da data");
        return -1;
    }

    strcpy(keyCopy, key);
    memcpy(valueCopy->data, value->data, value->datasize);
    valueCopy->datasize = value->datasize;
   
    int n = hash(keyCopy, table->numberLists);

    struct entry_t *possibleEntry;

    possibleEntry = list_get(table->lists[n], keyCopy);

    if (possibleEntry == NULL){
        list_add(table->lists[n], entry_create(keyCopy, valueCopy));
    } else {
        list_remove(table->lists[n], keyCopy);
        list_add(table->lists[n], entry_create(keyCopy, valueCopy));
    }

    return 0;
}

/* Função para obter da tabela o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou table_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da tabela,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função.
 * Devolve NULL em caso de erro.
 */
struct data_t *table_get(struct table_t *table, char *key){

    if(table == NULL || key == NULL){
        perror("A table ou a key são NULL");
        return NULL;
    }

    int n = hash(key, table->numberLists);

    struct entry_t *entry = list_get(table->lists[n], key);

    if (entry == NULL){
        struct data_t *data = (struct data_t *) malloc(sizeof(struct data_t));
        data->datasize = 0;
        data->data = NULL;
        return data;
    }

    struct data_t *value;
    
    value = (struct data_t *) malloc(sizeof(struct data_t));
    
    if(value == NULL){
        perror("Erro na criação da struct data_t");
        return NULL;
    }

    int tamanho = list_get(table->lists[n], key)->value->datasize;

    value->data = malloc(tamanho);
    if(value->data == NULL){
        perror("Erro na criação da data");
        return NULL;
    }

    memcpy(value->data, list_get(table->lists[n], key)->value->data , tamanho);
    value->datasize = tamanho;

    return value; 
}

/* Função para remover um elemento da tabela, indicado pela chave key, 
 * libertando toda a memória alocada na respetiva operação table_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int table_del(struct table_t *table, char *key){

    int n = hash(key, table->numberLists);

    struct entry_t *entry = list_get(table->lists[n], key);
    
    if(entry == NULL){
        return -1;
    } else {
        list_remove(table->lists[n], key);
        return 0;
    }
}

/* Função que devolve o número de elementos contidos na tabela.
 */
int table_size(struct table_t *table){
    
    if(table == NULL){
        perror("A table é NULL");
        return -1;
    }

    int nElements, i;
    nElements = 0;

    for(i = 0; i < table->numberLists; i++){
        nElements += list_size(table->lists[i]);
    }
    return nElements;
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **table_get_keys(struct table_t *table){

    int size = table_size(table); 

    char **keys = (char **) malloc(sizeof(char *) * (size + 1));

    if(keys == NULL){
        perror("Erro na criação das keys");
        return NULL;
    }

    int i, k;

    k = 0;

    for(i = 0; i < table->numberLists; i++){
        char **keysList = list_get_keys(table->lists[i]);
        int j = 0;
        while(keysList[j] != NULL){
            keys[k] = keysList[j];
            j++;
            k++;
        }
        free(keysList);
        
    }
    keys[k] = NULL;
    
    return keys;
    
}

/* Função que liberta toda a memória alocada por table_get_keys().
 */
void table_free_keys(char **keys){

    list_free_keys(keys);

}

/* Função que imprime o conteúdo da tabela.
 */
void table_print(struct table_t *table){

    int i;

    if(table != NULL){
        for(i = 0; i < table->numberLists; i++){
            list_print(table->lists[i]);
        }
    }
    
    

}

