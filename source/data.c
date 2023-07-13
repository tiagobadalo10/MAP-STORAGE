/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "data.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size 
 */
struct data_t *data_create(int size){
    
    if(size <= 0){
        perror("O size é menor ou igual a 0");
        return NULL;
    }

    struct data_t *p;

    p = (struct data_t *) malloc(sizeof(struct data_t));

    if(p == NULL){
        perror("Erro na criação da data");
        return NULL;
    }

    p->data = malloc(size);
    
    if(p->data == NULL){
        perror("Erro na criação da data da struct data");
        return NULL;
    }

    p->datasize = size;
    
    return p;
}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void *data){
    
    if(size <= 0 || data == NULL){
        return NULL;
    }

    struct data_t *p;

    p = (struct data_t *) malloc(sizeof(struct data_t));

    if(p == NULL){
        perror("Erro na criação da data");
        return NULL;
    }

    p->data = data;
    p->datasize = size;
    return p;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data){

    if(data != NULL){
        if(data->data != NULL){
            free(data->data);
        }
        free(data);
    }

}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 */
struct data_t *data_dup(struct data_t *data){
    
    if(data == NULL || data->datasize <= 0 || data->data == NULL){
        perror("A struct data_t ou a data da struct data_t são NULL ou o datasize é menor ou igual que 0");
        return NULL;
    }

    struct data_t *p;

    p = (struct data_t *) malloc(sizeof(struct data_t));

    if(p == NULL){
        perror("Erro na criação da data");
        return NULL;
    }

    p->data = malloc(data->datasize);
    memcpy(p->data, data->data, data->datasize);
    p->datasize = data->datasize;

    return p;
}

/* Função que substitui o conteúdo de um elemento de dados data_t.
*  Deve assegurar que destroi o conteúdo antigo do mesmo.
*/
void data_replace(struct data_t *data, int new_size, void *new_data){

    free(data->data);
    data->datasize = new_size;
    data->data = new_data;
}

