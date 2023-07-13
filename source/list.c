/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "list.h"
#include "data.h"
#include "entry.h"
#include "list-private.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Função que cria uma nova lista (estrutura list_t a ser definida pelo
 * grupo no ficheiro list-private.h).
 * Em caso de erro, retorna NULL.
 */
struct list_t *list_create(){
    
    struct list_t *p;

    p = (struct list_t *) malloc(sizeof(struct list_t));

    if(p == NULL){
        perror("Erro na criação da lista");
        return NULL;
    }

    p->head = NULL;

    return p;
}

/* Função que elimina uma lista, libertando *toda* a memoria utilizada
 * pela lista.
 */
void list_destroy(struct list_t *list){

    struct node_t *nextNode, *node;

    if(list != NULL){
        if(list->head != NULL){
            node = list->head;
            while(node != NULL){
                nextNode = node->next;
                if(node->value != NULL && node->value->key != NULL && node->value->value->data != NULL){
                    entry_destroy(node->value);
                }
                free(node);
                node = nextNode;
            }
            
        }
        free(list);
    }
}

/* Função que adiciona no final da lista (tail) a entry passada como
* argumento caso não exista na lista uma entry com key igual àquela
* que queremos inserir.
* Caso exista, os dados da entry (value) já existente na lista serão
* substituídos pelos os da nova entry.
* Retorna 0 (OK) ou -1 (erro).
*/

int list_add(struct list_t *list, struct entry_t *entry){


    if(list == NULL){
        perror("A lista é NULL");
        return -1;
    }
    
    int result = 0;
    struct node_t *nextNode, *node;

    node = (struct node_t *) malloc(sizeof(struct node_t));
    

    if(node == NULL){
        perror("Erro na criação do node");
        return -1;
    }

    node->value = entry;
    node->next = NULL;

    if(list->head == NULL){ 
        list->head = node;
    }
    else{
        nextNode = list->head;
        while(nextNode != NULL){
            result = strcmp(entry->key, nextNode->value->key);
            if(result == 0){
                entry_destroy(nextNode->value);
                nextNode->value = entry;
                free(node);
                return result;
            }
            if(nextNode->next == NULL){
                break;
            }
            nextNode = nextNode->next;
        }
        nextNode->next = node; 
    }

    return 0;
}

/* Função que elimina da lista a entry com a chave key.
 * Retorna 0 (OK) ou -1 (erro).
 */
int list_remove(struct list_t *list, char *key){

    int i, result, indexEntry;
    struct node_t *nextNode, *delNode, *succNode, *nextNextNode;
    
    if(list == NULL || key == NULL){
        perror("A lista ou a key são NULL");
        return -1;
    }

    if(list->head != NULL){
        nextNode = list->head;
        nextNextNode = nextNode->next;
        if(strcmp(key,list->head->value->key) == 0){
            list->head = nextNextNode;
            entry_destroy(nextNode->value);
            free(nextNode);
            return 0;
        }

    }
   
    i = 0;
    indexEntry = -1;
    result = -1;
    while(nextNode != NULL){
        result = strcmp(key, nextNode->value->key);
        if(result == 0){
            indexEntry = i;
            break;
        }
        if(nextNode->next != NULL){
            nextNode = nextNode->next;
        }

        i++;
    }
    if(indexEntry == -1){
        perror("Erro a remover a chave");
        return -1;
    }

    nextNode = list->head;
    i = 0;
    while(nextNode != NULL){
        if(i == indexEntry - 1){ 
            delNode = nextNode->next; 
            if(delNode->next != NULL){
                succNode = delNode->next;
                nextNode->next = succNode; 
            }
            else{
                nextNode->next = NULL;
            }
            entry_destroy(delNode->value);
            free(delNode);
        }
        nextNode = nextNode->next;
        i++;
    }
    return 0;
    
}

/* Função que obtém da lista a entry com a chave key.
 * Retorna a referência da entry na lista ou NULL em caso de erro.
 * Obs: as funções list_remove e list_destroy vão libertar a memória
 * ocupada pela entry ou lista, significando que é retornado NULL
 * quando é pretendido o acesso a uma entry inexistente.
*/
struct entry_t *list_get(struct list_t *list, char *key){
    
    if(list == NULL || key == NULL){
        perror("A lista ou a key são NULL");
        return NULL;
    }

    struct node_t *nextNode;
    int result;

 
    nextNode = list->head;
    while(nextNode != NULL){
        result = strcmp(key, nextNode->value->key);
        if(result == 0){
            return nextNode->value;
        }
        nextNode = nextNode->next;
    }
    return NULL;
}

/* Função que retorna o tamanho (número de elementos (entries)) da lista,
 * ou -1 (erro).
 */
int list_size(struct list_t *list){
    
    if(list == NULL){
        perror("A lista é NULL");
        return -1;
    }

    struct node_t *nextNode = list->head;
    int counter = 0;

    while(nextNode != NULL){
        nextNode = nextNode->next;
        counter++;
    }
    return counter;
}

/* Função que devolve um array de char* com a cópia de todas as keys da 
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **list_get_keys(struct list_t *list){
    
    int tamanho, i = 0;
    char **keys;
    struct node_t *nextNode;

    tamanho = list_size(list) + 1;
    keys = (char **) malloc(sizeof(char *) * tamanho);

    nextNode = list->head;
    while(nextNode != NULL){
        keys[i] = (char *) malloc(strlen(nextNode->value->key) +1);
        strcpy(keys[i], nextNode->value->key);
        nextNode = nextNode->next;
        i++;
    }
    keys[i] = NULL;
    return keys;
}

/* Função que liberta a memória ocupada pelo array das keys da tabela,
 * obtido pela função list_get_keys.
 */
void list_free_keys(char **keys){
    
    int i = 0;

    if(keys != NULL){
        while(keys[i] != NULL){
            free(keys[i]);
            i++;
        }
        
        free(keys);
    }
    
}

/* Função que imprime o conteúdo da lista para o terminal.
 */
void list_print(struct list_t *list){
    
    struct node_t *nextNode;

    nextNode = list->head;
    while(nextNode != NULL){
        printf("KEY - %s | DATA - %s | DATASIZE - %d \n", nextNode->value->key, (char *) nextNode->value->value->data,  nextNode->value->value->datasize);
        nextNode = nextNode->next;
    }
    
}
