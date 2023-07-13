/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "client_stub.h"
#include "client_stub-private.h"
#include "message.h"
#include "network_client.h"
#include "message-private.h"
#include "sdmessage.pb-c.h"
#include "stats-private.h"
#include "inet.h"
#include "zookeeper/zookeeper.h"

/* Remote table, que deve conter as informações necessárias para estabelecer a comunicação com o servidor. A definir pelo grupo em client_stub-private.h
 */
struct rtable_t;
struct MessageT msg;

/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtable_t *rtable_connect(const char *address_port){

    int res;
    char *token;

    struct rtable_t *rtable = (struct rtable_t *) malloc(sizeof(struct rtable_t));

    if(rtable == NULL){
        perror("Erro na criação da rtable.\n");
        return NULL;
    }
    
    token = strtok((char *) address_port, ":");

    rtable->serverhostname = (char *) malloc(strlen(token) + 1);

    if(rtable->serverhostname == NULL){
        perror("Erro na criação do serverhostname da rtable.\n");
        return NULL;
    }

    memcpy(rtable->serverhostname, token, strlen(token) + 1);

    token = strtok(NULL, ":");
    rtable->server.sin_port = htons(atoi(token));

    res = network_connect(rtable);

    if(res == -1){
        free(rtable);
        perror("Erro na conexão com o servidor.\n");
        return NULL;
    }

    return rtable;
}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtable_disconnect(struct rtable_t *rtable){

    if(rtable == NULL){
        perror("A rtable é NULL.\n");
        return -1;
    }

    int res = network_close(rtable);

    if(res == -1){
       perror("Erro a fechar a ligação.\n");
       return -1;
    }

    if(rtable->serverhostname != NULL){
        free(rtable->serverhostname);
    }

    free(rtable);

    return 0;
}

/* Função para adicionar um elemento na tabela.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry){

    message__t__init(&msg);

    if(rtable == NULL){
        perror("A rtable é NULL.\n");
        return -1;
    }

    MessageT__Opcode OP_PUT = MESSAGE__T__OPCODE__OP_PUT;
    MessageT__CType CT_ENTRY = MESSAGE__T__C_TYPE__CT_ENTRY;
    MessageT__Opcode OP_ERROR = MESSAGE__T__OPCODE__OP_ERROR;

    ProtobufCBinaryData data;

    data.len = entry->value->datasize;
    data.data = malloc(entry->value->datasize);
    memcpy(data.data, entry->value->data, data.len);

    msg.opcode = OP_PUT;
    msg.c_type = CT_ENTRY;
    msg.data = data;
    msg.key = entry->key;

    struct MessageT *answer;
    
    answer = network_send_receive(rtable, &msg);

    if(answer->opcode == OP_ERROR){
        return -1;
    }

    free(data.data);
    message__t__free_unpacked(answer, NULL);

    return 0;
}

/* Função para obter um elemento da tabela.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtable_get(struct rtable_t *rtable, char *key){

    message__t__init(&msg);

    if(rtable == NULL){
        perror("A rtable é NULL.\n");
        return NULL;
    }

    if(key == NULL){
        perror("A key é NULL.\n");
        return NULL;
    }

    MessageT__Opcode OP_GET = MESSAGE__T__OPCODE__OP_GET;
    MessageT__CType CT_KEY = MESSAGE__T__C_TYPE__CT_KEY;
    MessageT__Opcode OP_ERROR = MESSAGE__T__OPCODE__OP_ERROR;

    msg.opcode = OP_GET;
    msg.c_type = CT_KEY;
    msg.key = key;

    struct MessageT *answer;

    answer = network_send_receive(rtable, &msg);

    if(answer->opcode == OP_ERROR){
        perror("Erro na realização do comando.\n");
        return NULL;
    }

    if(answer->data.data == NULL && answer->data.len == 0){
        struct data_t *dataT = (struct data_t *) malloc(sizeof(struct data_t));
        dataT->data = NULL;
        dataT->datasize = 0;
        message__t__free_unpacked(answer, NULL);
        return dataT;
    }

    struct data_t *dataT = data_create2(answer->data.len, answer->data.data);
    struct data_t *data = data_dup(dataT);

    message__t__free_unpacked(answer, NULL);

    return data;
}


/* Função para remover um elemento da tabela. Vai libertar 
 * toda a memoria alocada na respetiva operação rtable_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtable_del(struct rtable_t *rtable, char *key){

    message__t__init(&msg);

    if(rtable == NULL){
        perror("A rtable é NULL.\n");
        return -1;
    }

    if(key == NULL){
        perror("A key é NULL.\n");
        return -1;
    }

    MessageT__Opcode OP_DEL = MESSAGE__T__OPCODE__OP_DEL;
    MessageT__CType CT_KEY = MESSAGE__T__C_TYPE__CT_KEY;

    msg.opcode = OP_DEL;
    msg.c_type = CT_KEY;
    msg.key = key;

    MessageT__Opcode OP_ERROR = MESSAGE__T__OPCODE__OP_ERROR;
    
    struct MessageT *answer;

    answer = network_send_receive(rtable, &msg);

    if(answer->opcode == OP_ERROR){
        message__t__free_unpacked(answer, NULL);
        return -1;
    }

    message__t__free_unpacked(answer, NULL);

    return 0;
}

/* Devolve o número de elementos contidos na tabela.
 */
int rtable_size(struct rtable_t *rtable){

    message__t__init(&msg);

    int size;

    if(rtable == NULL){
        perror("A rtable é NULL");
        return -1;
    }

    MessageT__Opcode OP_SIZE = MESSAGE__T__OPCODE__OP_SIZE;
    MessageT__CType CT_NONE = MESSAGE__T__C_TYPE__CT_NONE;

    msg.opcode = OP_SIZE;
    msg.c_type = CT_NONE;

    struct MessageT *answer;

    answer = network_send_receive(rtable, &msg);

    size = answer->size;

    message__t__free_unpacked(answer, NULL);

    return size;
}

/* Devolve um array de char* com a cópia de todas as keys da tabela,
 * colocando um último elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable){

    message__t__init(&msg);

    if(rtable == NULL){
        perror("A rtable é NULL.\n");
        return NULL;
    }

    MessageT__Opcode OP_GETKEYS = MESSAGE__T__OPCODE__OP_GETKEYS;
    MessageT__CType CT_NONE = MESSAGE__T__C_TYPE__CT_NONE;

    msg.opcode = OP_GETKEYS;
    msg.c_type = CT_NONE;

    struct MessageT *answer;

    answer = network_send_receive(rtable, &msg);

    if(answer == NULL){
        return NULL;
    }

    int i = 0; 
    char **keys = (char **) malloc(sizeof(char *) * answer->n_info);

    if(answer->n_info > 0){
        while(i < answer->n_info){
            keys[i] = strdup(answer->info[i]);
            i++;
        }      
   
        keys[i] = NULL;
        return keys;  
    }

    return NULL;

    
}

/* Liberta a memória alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys){

    int i = 0;

    if(keys != NULL){
        while(keys[i] != NULL){
            free(keys[i]);
            i++;
        }
        
        free(keys);
    }
}

/* Função que imprime o conteúdo da tabela remota para o terminal.
 */
void rtable_print(struct rtable_t *rtable){

    message__t__init(&msg);

    MessageT__Opcode OP_PRINT = MESSAGE__T__OPCODE__OP_PRINT;
    MessageT__CType CT_NONE = MESSAGE__T__C_TYPE__CT_NONE;

    msg.opcode = OP_PRINT;
    msg.c_type = CT_NONE;

    struct MessageT *answer;
    int i = 0;

    answer = network_send_receive(rtable, &msg);

    if(answer->size != -1){
        while(answer->info[i] != NULL){
        printf("%s\n", answer->info[i]);
        i++;
        }
        message__t__free_unpacked(answer, NULL);
    }

    else{
        printf("Tabela vazia.\n");
    }

    
}

/* Obtém as estatísticas do servidor.
*/

struct statistics *rtable_stats(struct rtable_t *rtable){

    message__t__init(&msg);

    MessageT__Opcode OP_STATS = MESSAGE__T__OPCODE__OP_STATS;
    MessageT__CType CT_NONE = MESSAGE__T__C_TYPE__CT_NONE;

    msg.opcode = OP_STATS;
    msg.c_type = CT_NONE;

    struct MessageT *answer;
    
    answer = network_send_receive(rtable, &msg);

    struct statistics *stats = (struct statistics *) malloc(sizeof(struct statistics));

    stats->n_times_DEL = answer->n_times_del;
    stats->n_times_GET = answer->n_times_get;
    stats->n_times_GETKEYS = answer->n_times_getkeys;
    stats->n_times_PRINT = answer->n_times_print;
    stats->n_times_PUT = answer->n_times_put;
    stats->n_times_SIZE = answer->n_times_size;
    stats->time = answer->avg_time;

    message__t__free_unpacked(answer, NULL);

    return stats;

}
