/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "serialization.h"
#include "data.h"
#include "entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Serializa uma estrutura data num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho do buffer
 * alocado ou -1 em caso de erro.
 */
int data_to_buffer(struct data_t *data, char **data_buf){

    if(data == NULL || data_buf == NULL){
        perror("A struct data_t ou o data_buf são NULL");
        return -1;
    }

    int size;

    size = sizeof(int) + data->datasize;

    *data_buf = (char *) malloc(size);

    if(*data_buf == NULL){
        perror("Erro na criação do primeiro indice do data_buf");
        return -1;
    }  

    memcpy(*data_buf, &size, sizeof(int));

    memcpy(*data_buf + sizeof(int) , data->data, data->datasize);
    

    return size;

}

/* De-serializa a mensagem contida em data_buf, com tamanho
 * data_buf_size, colocando-a e retornando-a numa struct
 * data_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct data_t *buffer_to_data(char *data_buf, int data_buf_size){

    if(data_buf == NULL || data_buf_size < 1){
        perror("O data_buf é NULL ou o data_buf_size é menor que 1");
        return NULL;
    }  

    struct data_t *data = data_create(data_buf_size - sizeof(int));

    int size;

    memcpy(&size, data_buf, sizeof(int));

    memcpy(data->data , data_buf + sizeof(int), data->datasize);



    return data;

}

/* Serializa uma estrutura entry num buffer que sera alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer alocado ou -1 em caso de erro.
 */
int entry_to_buffer(struct entry_t *data, char **entry_buf){

    if(data == NULL  || entry_buf == NULL){
        perror("A struct entry_t ou o entry_buf são NULL");
        return -1;
    }

    int size, datasize, keysize;

    size = strlen(data->key) + 1 + sizeof(int) + sizeof(int) + data->value->datasize; 

    datasize = data->value->datasize;
    
    keysize = strlen(data->key) + 1;

    *entry_buf = (char *) malloc(size);

    if(*entry_buf == NULL){
        perror("Erro na criação do primeiro indice do entry_buf");
        return -1;
    }

    memcpy(*entry_buf , &datasize, sizeof(int));

    memcpy(*entry_buf + sizeof(int), data->value->data , data->value->datasize);

    memcpy(*entry_buf + data->value->datasize + sizeof(int), data->key , strlen(data->key) + 1);

    memcpy(*entry_buf + data->value->datasize + sizeof(int) + strlen(data->key) + 1, &keysize , sizeof(int));

    return size;

}

/* De-serializa a mensagem contida em entry_buf, com tamanho
 * entry_buf_size, colocando-a e retornando-a numa struct
 * entry_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct entry_t *buffer_to_entry(char *entry_buf, int entry_buf_size){
    
    if(entry_buf == NULL || entry_buf_size < 1){
        perror("O entry_buf é NULL ou o entry_buf_size é menor que 1");
        return NULL;
    }

    int size, sizeKey, keysize = 0;

    memcpy(&size, entry_buf, sizeof(int)); // entry_buf dá o datasize

    struct data_t *data = data_create(size);

    memcpy(data->data , entry_buf + sizeof(int), data->datasize);

    sizeKey = entry_buf_size - size - sizeof(int) - sizeof(int);

    char *key = malloc(sizeKey);

    memcpy(key, entry_buf + sizeof(int) + data->datasize, sizeKey);

    struct entry_t *entry = entry_create(key, data);

    memcpy(&keysize, entry_buf + sizeof(int) + data->datasize + strlen(key) + 1, sizeof(int));
    
    return entry;
}