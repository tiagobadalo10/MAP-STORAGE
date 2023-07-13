/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "sdmessage.pb-c.h"
#include "table_skel.h"
#include "table.h"
#include "table-private.h"
#include "list-private.h"
#include "table_skel-private.h"
#include "inet.h"
#include "stats-private.h"
#include <pthread.h>
#include "zookeeper/zookeeper.h"
#include <errno.h>
#include "client_stub.h"
#include "client_stub-private.h"

#define ZDATALEN 1024 * 1024

typedef struct String_vector zoo_string; 

int numberLists, boolean, isprimary;

struct table_t *table;               // Variável partilhada
struct statistics *stats;            // Variável partilhada

int *counter_LISTAS;                 // Variável de estado
int counter_ESCRITA_ATUAL = 0;       // Variável de estado
int counter_LEITURA_ATUAL = 0;       // Variável de estado
int counter_PUT_ATUAL = 0;           // Variável de estado
int counter_DEL_ATUAL = 0;           // Variável de estado
int counter_ESCRITORES_ESPERA = 0;   // Variável de estado

pthread_mutex_t m_stats = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	c_stats = PTHREAD_COND_INITIALIZER;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	c = PTHREAD_COND_INITIALIZER;

static zhandle_t *zh;
static int connected;

static char* zoo_root = "/kvstore"; 
static char *watcher_ctx = "ZooKeeper Data Watcher";
char *primary_path = "/kvstore/primary";
char *backup_path = "/kvstore/backup";

char *primary_host_ip;
char *backup_host_ip;

struct rtable_t *rtable;


/* Função de controlo de entrada da leitura das stats
 *
 */
void comeca_leitura_stats(){

    pthread_mutex_lock(&m_stats);
    while(counter_ESCRITA_ATUAL == 1 || counter_ESCRITORES_ESPERA > 0){
        pthread_cond_wait(&c_stats, &m_stats);
    }
    counter_LEITURA_ATUAL++;
    pthread_mutex_unlock(&m_stats);
}

/* Função de controlo de saída da leitura das stats
 *
 */
void termina_leitura_stats(){

    pthread_mutex_lock(&m_stats);
    counter_LEITURA_ATUAL--;

    if(counter_LEITURA_ATUAL == 0){
        pthread_cond_broadcast(&c_stats);
    }

    pthread_mutex_unlock(&m_stats);
}

/* Função de controlo de entrada da escrita das stats
 *
 */
void comeca_escrita_stats() {

    pthread_mutex_lock(&m_stats);
    counter_ESCRITORES_ESPERA++;

    while(counter_LEITURA_ATUAL > 0 || counter_ESCRITA_ATUAL == 1){
        pthread_cond_wait(&c_stats, &m_stats);
    }
    
    counter_ESCRITA_ATUAL = 1;
    counter_ESCRITORES_ESPERA--;
    pthread_mutex_unlock(&m_stats);
}

/* Função de controlo de saída da leitura das stats
 *
 */
void termina_escrita_stats(){

    pthread_mutex_unlock(&m_stats);
    counter_ESCRITA_ATUAL = 0;
        pthread_cond_broadcast(&c_stats);
    pthread_mutex_unlock(&m_stats);
}

/* Função de controlo de entrada nas listas da table
 * Valor do code pode ser 0 (PUT), 1 (DEL), 2 (GET)
 */
void comeca_listas(int hashcode, int code){

    pthread_mutex_lock(&m);

    if(code == 2){

        while(counter_LISTAS[hashcode] != 0){
            pthread_cond_wait(&c, &m);
        }

        counter_LISTAS[hashcode] = 1;

        if(code == 0){
            counter_PUT_ATUAL++;
        }
        else if(code == 1){
            counter_DEL_ATUAL++;
        }   
    
        pthread_mutex_unlock(&m);
    }
    else{

        while(counter_LISTAS[hashcode] != 0 && boolean != 1){
            pthread_cond_wait(&c, &m);
        }

        counter_LISTAS[hashcode] = 1;

        if(code == 0){
            counter_PUT_ATUAL++;
        }
        else if(code == 1){
            counter_DEL_ATUAL++;
        }   
    
        pthread_mutex_unlock(&m);
    }

}

/* Função de controlo de saída nas listas da table
 * Valor do code pode ser 0 (PUT), 1 (DEL), 2 (GET)
 */
void termina_listas(int hashcode, int code){

    pthread_mutex_lock(&m);

    counter_LISTAS[hashcode] = 0;
    if(code == 0){
        counter_PUT_ATUAL--;
    }
    else if(code == 1){
        counter_DEL_ATUAL--;
    }
        pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&m);
}


/* Função de controlo de entrada de leitura da table
 *
 */
void comeca(){

    pthread_mutex_lock(&m);
    while(counter_PUT_ATUAL != 0 || counter_DEL_ATUAL != 0){
        pthread_cond_wait(&c, &m);
    }
    pthread_mutex_unlock(&m);

}

/* Função de controlo de saída de leitura da table
 *
 */
void termina(){

    pthread_mutex_lock(&m);
    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&m);

}


/* Função que dá watch da mudança de estado de conexão
 *
 */
void connection_watcher(zhandle_t *zzh, int type, int state, const char *path, void *context){

    if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
			connected = 1; 
		} else {
			connected = 0; 
		}
	} 
}

/* Função que dá watch aos filhos de /kvstore
 *
 */
static void child_watcher(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx){

    zoo_string* children_list = (zoo_string *) malloc(sizeof(zoo_string));
    int retvalue, size, i;
    char *buff;

    size = ZDATALEN;

    if(children_list == NULL){
        perror("Erro na criação da children list.\n");
        exit(EXIT_FAILURE);
    }

    if(state == ZOO_CONNECTED_STATE){

        if(type == ZOO_CHILD_EVENT){

            if(ZOK != zoo_wget_children(zh, zoo_root, child_watcher, watcher_ctx, children_list)){
                perror("Erro a dar watch aos filhos de /kvstore.\n");
                exit(EXIT_FAILURE);
            }    

            if(children_list->count == 1){

                retvalue = zoo_exists(zh, primary_path, 0, NULL); 

                if(ZOK == retvalue){ // Se for servidor primário e o backup tiver saído

                    boolean = 1; // não aceita mais pedidos de escrita dos clientes até que volte a haver backup
                }

                retvalue = zoo_exists(zh, backup_path, 0, NULL);

                if(ZOK == retvalue){ // Se for servidor backup e o primário tiver saído

                    buff = (char *) malloc(size);

                    if(buff == NULL){
                        perror("Erro ao criar o buff.\n");
                        exit(EXIT_FAILURE);
                    }

                    char path[128] = "/kvstore";
                    strcat(path, "/");
                    strcat(path, children_list->data[0]);

                    retvalue = zoo_get(zh, path, 0, buff, &size, NULL);

                    if(ZOK != retvalue){
                        perror("Erro a dar get dos metadados do path.\n");
                        exit(EXIT_FAILURE);
                    }
                    
                    retvalue = zoo_create(zh, primary_path, buff, strlen(buff) + 1, & ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, NULL, 0); // autopromove-se a servidor primário

                    if(ZOK != retvalue){
                        perror("Erro a criar ZNode /kvstore/primary.\n");
                        exit(EXIT_FAILURE);
                    }

                    retvalue = zoo_delete(zh, path, 0);

                    if(ZOK != retvalue){
                        perror("Erro a dar delete do Znode da path.\n");
                        exit(EXIT_FAILURE);
                    }

                    isprimary = 1;

                    free(buff);
                }
            }

            if(children_list->count == 2){
                
                if(isprimary){ 

                    buff = (char *) malloc(size);

                    if(buff == NULL){
                        perror("Erro ao criar o buff.\n");
                        exit(EXIT_FAILURE);
                    }

                    retvalue = zoo_get(zh, backup_path, 0, buff, &size, NULL);

                    if(ZOK != retvalue){
                        perror("Erro a dar get dos metadados do /kvstore/backup.\n");
                        exit(EXIT_FAILURE);
                    }
                  
                    rtable = rtable_connect(buff);

                    comeca();

                    char **keys = table_get_keys(table);

                    int size = table_size(table);

                    termina();


                    for(i = 0; i < size; i++){

                        int hashCode = hash(keys[i], numberLists);

                        comeca_listas(hashCode, 2);

                        struct data_t *data = table_get(table, keys[i]);

                        termina_listas(hashCode, 2);
                        
                        struct entry_t *entry = entry_create(keys[i], data);

                        retvalue = rtable_put(rtable, entry);

                        if(retvalue == -1){
                            perror("Erro a dar put na tabela do /kvstore/backup.\n");
                            exit(EXIT_FAILURE);
                        }
                    }
       
                    boolean = 0;

                    free(buff);
                    
                }  
            }
        }
    }

    free(children_list);
}


/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int table_skel_init(int n_lists, char *port_ip_server, char *port){

    int i, retvalue, retvalue1, size;

    if (n_lists < 1){
        perror("O número de listas é menor que 1,\n");
        return -1;
    }

    numberLists = n_lists;
    size = ZDATALEN;

    counter_LISTAS = (int *) malloc(numberLists * sizeof(int));

    for(i = 0; i < numberLists; i++){
        counter_LISTAS[i] = 0;
    }

    table = table_create(n_lists);

    if(table == NULL){
        perror("Erro na criação da table.\n");
        return -1;
    }


    zh = zookeeper_init(port_ip_server, connection_watcher, 2000, 0, 0, 0); // Ligar ao ZooKeeper 

	if (zh == NULL)	{
		fprintf(stderr, "Erro a conectar ao ZooKeeper[%d].\n", errno);
	    return -1;
	}

    sleep(3);


    zoo_string* children_list = (zoo_string *) malloc(sizeof(zoo_string));

    if(children_list == NULL){
        perror("Erro na criação da children list.\n");
        return -1;
    }

    if(connected){

        retvalue = zoo_exists(zh, zoo_root, 0, NULL);

        if(ZNONODE == retvalue){ // Se não existir o Znode /kvstore

            retvalue = zoo_create(zh, zoo_root, NULL, -1,  & ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0); // criar esse Znode normal

            if(ZOK != retvalue){ 
                perror("Error a criar znode /kvstore.\n");
                return -1;
            }

            retvalue = zoo_create(zh, primary_path, get_host_port(port), strlen(get_host_port(port)) + 1, & ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, NULL, 0); // criar o nó efémero /kvstore/primary, assumindo-se como servidor primário

            if(ZOK != retvalue){ 
                perror("Error a criar znode /kvstore/primary.\n");
                return -1;
            }

            isprimary = 1;
            backup_host_ip = NULL; 

            retvalue = zoo_wget_children(zh, zoo_root, &child_watcher, watcher_ctx , children_list);

            if (ZOK != retvalue) { // Obter e fazer watch aos filhos de /kvstore
                perror("Erro a dar watch aos filhos de /kvstore.\n");
                return -1;
            }

            return 0;
           
        } 

        retvalue = zoo_exists(zh, zoo_root, 0, NULL);

        if(ZOK == retvalue){ // Se o Znode /kvstore existir

            retvalue = zoo_exists(zh, primary_path, 0, NULL);
            retvalue1 = zoo_exists(zh, backup_path, 0, NULL);

            if(ZOK == retvalue && ZOK == retvalue1){ // tiver nós filhos /primary e /backup
                return -1;
            }

            if(ZNONODE == retvalue && ZNONODE == retvalue1){ // Se não existirem nós filhos de /kvstore
                
                retvalue = zoo_create(zh, primary_path, get_host_port(port), strlen(get_host_port(port)) + 1, & ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, NULL, 0);

                if(ZOK != retvalue){ //  criar o nó efémero /kvstore/primary, assumindo-se como servidor primário;
                    perror("Erro a criar znode /kvstore/primary.\n");
                    return -1;
                }
                
                isprimary = 1;
                backup_host_ip = NULL;

                retvalue = zoo_wget_children(zh, zoo_root, &child_watcher, watcher_ctx , children_list);

                if (ZOK != retvalue) { // Obter e fazer watch aos filhos de /kvstore
                    perror("Erro a dar watch aos filhos de /kvstore.\n");
                    return -1;
                }

                return 0;
            }

            retvalue = zoo_exists(zh, primary_path, 0, NULL);
            retvalue1 =  zoo_exists(zh, backup_path, 0, NULL);

            if(ZOK == retvalue && ZNONODE == retvalue1){ // Se existir o nó filho /primary e não existir o nó filho /backup

                retvalue = zoo_create(zh, backup_path, get_host_port(port), strlen(get_host_port(port)) + 1, & ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, NULL, 0);
            
                if(ZOK != retvalue){ // criar o nó efémero /kvstore/backup, assumindo-se como servidor de backup;
                    perror("Erro a criar znode /kvstore/backup.\n");
                    return -1;
                }               

                primary_host_ip = (char *) malloc(size); 
                
                if(primary_host_ip == NULL){
                    perror("Erro a criar primary_host_ip.\n");
                    return -1;
                }

                retvalue = zoo_get(zh, primary_path, 0, primary_host_ip, &size, NULL);
            
                if(ZOK != retvalue){
                    perror("Erro a dar get dos metadados do /kvstore/primary.\n");
                    return -1;
                }

                isprimary = 0;
            }

            retvalue = zoo_exists(zh, primary_path, 0, NULL);
            retvalue1 = zoo_exists(zh, backup_path, 0, NULL);

            if(ZNONODE == retvalue && ZOK == retvalue1){ // Se existir o nó filho /backup e não existir o nó filho /primary
                sleep(3);
                isprimary = 0;
                table_skel_init(n_lists, port_ip_server, port);
            }

            retvalue = zoo_wget_children(zh, zoo_root, &child_watcher, watcher_ctx , children_list);

            if (ZOK != retvalue) { // Obter e fazer watch aos filhos de /kvstore
                perror("Erro a dar watch aos filhos de /kvstore.\n");
                return -1;
            }
        }  
    }

    return 0;
}

/* Liberta toda a memória e recursos alocados pela função table_skel_init.
 */
void table_skel_destroy(){

    free(counter_LISTAS);

    table_destroy(table);

}

/* Executa uma operação na tabela (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura MessageT para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, tabela nao incializada)
*/
int invoke(struct MessageT *msg){

    if(isprimary){

        if(msg == NULL){
            perror("A mensagem é NULL.\n");
            return -1;
        }

    MessageT__Opcode opCode = msg->opcode;
    MessageT__CType cType =  msg->c_type;
    int resultado, retvalue;

    MessageT__Opcode OP_SIZE = MESSAGE__T__OPCODE__OP_SIZE;
    MessageT__Opcode OP_DEL = MESSAGE__T__OPCODE__OP_DEL;
    MessageT__Opcode OP_GET = MESSAGE__T__OPCODE__OP_GET;
    MessageT__Opcode OP_PUT = MESSAGE__T__OPCODE__OP_PUT;
    MessageT__Opcode OP_GETKEYS = MESSAGE__T__OPCODE__OP_GETKEYS;
    MessageT__Opcode OP_PRINT = MESSAGE__T__OPCODE__OP_PRINT;
    MessageT__Opcode OP_STATS =  MESSAGE__T__OPCODE__OP_STATS;
    MessageT__Opcode OP_ERROR = MESSAGE__T__OPCODE__OP_ERROR;

    MessageT__CType CT_KEY = MESSAGE__T__C_TYPE__CT_KEY;
    MessageT__CType CT_VALUE = MESSAGE__T__C_TYPE__CT_VALUE;
    MessageT__CType CT_ENTRY = MESSAGE__T__C_TYPE__CT_ENTRY;
    MessageT__CType CT_KEYS = MESSAGE__T__C_TYPE__CT_KEYS;
    MessageT__CType CT_RESULT = MESSAGE__T__C_TYPE__CT_RESULT;
    MessageT__CType CT_TABLE = MESSAGE__T__C_TYPE__CT_TABLE;
    MessageT__CType CT_NONE = MESSAGE__T__C_TYPE__CT_NONE;

    retvalue = zoo_exists(zh, backup_path, 0, NULL);

    if(opCode == OP_SIZE && cType == CT_NONE){ // size
        msg->opcode = OP_SIZE + 1;
        msg->c_type = CT_RESULT;

        comeca();
        
        resultado = table_size(table);

        termina();

        comeca_leitura_stats();

        int n_times_SIZE = stats->n_times_SIZE;

        termina_leitura_stats();

        n_times_SIZE++;

        comeca_escrita_stats();

        stats->n_times_SIZE = n_times_SIZE;

        termina_escrita_stats();
         
        msg->size = resultado;
    }

    if(opCode == OP_DEL && cType == CT_KEY){ // del <key>

        if(ZOK != retvalue){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }

        int hashCode = hash(msg->key, numberLists);

        comeca_listas(hashCode, 1);

        resultado = rtable_del(rtable, msg->key);

        if(resultado == -1){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }
         
        resultado = table_del(table, msg->key);

        termina_listas(hashCode, 1);

        comeca_leitura_stats();

        int n_times_DEL = stats->n_times_DEL;

        termina_leitura_stats();

        n_times_DEL++;

        comeca_escrita_stats();

        stats->n_times_DEL = n_times_DEL;

        termina_escrita_stats();

        if(resultado == -1){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }
        
        msg->opcode = OP_DEL + 1;
        msg->c_type = CT_NONE;
        
        
    }

    if(opCode == OP_GET && cType == CT_KEY){ // get <key>

        int hashCode = hash(msg->key, numberLists);

        comeca_listas(hashCode, 2);

        struct data_t *data = table_get(table, msg->key);
    
        termina_listas(hashCode, 2);

        comeca_leitura_stats();

        int n_times_GET = stats->n_times_GET;

        termina_leitura_stats();

        n_times_GET++;

        comeca_escrita_stats();

        stats->n_times_GET = n_times_GET;

        termina_escrita_stats();

        if(data == NULL){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }

        msg->opcode = OP_GET + 1;
        msg->c_type = CT_VALUE;
        msg->data.data = data->data;
        msg->data.len = data->datasize;  

    }

    if(opCode == OP_PUT && cType == CT_ENTRY){ // put <key> <data>

        if(ZOK != retvalue){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }

        struct data_t *data;
        
        data = data_create2(msg->data.len, msg->data.data);

        int hashCode = hash(msg->key, numberLists);

        comeca_listas(hashCode, 0); 

        struct entry_t *entry = entry_create(msg->key, data);
        
        resultado = rtable_put(rtable, entry);

        if(resultado == -1){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }

        resultado = table_put(table, msg->key, data);

        termina_listas(hashCode, 0);

        comeca_leitura_stats();

        int n_times_PUT = stats->n_times_PUT;

        termina_leitura_stats();

        n_times_PUT++;

        comeca_escrita_stats();

        stats->n_times_PUT = n_times_PUT;

        termina_escrita_stats();

        if(resultado == -1){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }
        else{
            msg->opcode = OP_PUT + 1;
            msg->c_type = CT_NONE;
        }

    }

    if(opCode == OP_GETKEYS && cType == CT_NONE){ // getkeys

        msg->opcode = OP_GETKEYS + 1;
        msg->c_type = CT_KEYS;

        comeca_leitura_stats();

        int n_times_GETKEYS = stats->n_times_GETKEYS;

        termina_leitura_stats();

        n_times_GETKEYS++;

        comeca_escrita_stats();

        stats->n_times_GETKEYS = n_times_GETKEYS;

        termina_escrita_stats();

        comeca();
        
        msg->n_info = table_size(table);
        msg->info = table_get_keys(table);

        termina();
    }

    if(opCode == OP_PRINT && cType == CT_NONE){ // table_print

        int i, j;
        char **print; 

        msg->opcode = OP_PRINT + 1;
        msg->c_type = CT_TABLE;

        comeca_leitura_stats();

        int n_times_PRINT = stats->n_times_PRINT;

        termina_leitura_stats();

        n_times_PRINT++;

        comeca_escrita_stats();

        stats->n_times_PRINT = n_times_PRINT;

        termina_escrita_stats();

        j = 0;

        comeca();

        if(table_size(table) != 0){

            print = (char **) malloc(sizeof(char *) * table_size(table));

            for(i = 0; i < table_size(table); i++){
                if(table_get_keys(table)[i] != NULL){
                    char *p = (char *) table_get(table, table_get_keys(table)[i])->data;
                    print[j] = (char *) malloc(snprintf(NULL, 0, "key - %s | data - %s | datasize - %d \n", table_get_keys(table)[i], p, table_get(table, table_get_keys(table)[i])->datasize));
                    sprintf(print[j], "key - %s | data - %s | datasize - %d", table_get_keys(table)[i], p, table_get(table, table_get_keys(table)[i])->datasize);
                    j++;
                }
            }

            msg->n_info = table_size(table);
            msg->info = print; 
        }
        else{
            msg->size = -1;
        }  

        termina(); 
    
    }

    if(opCode == OP_STATS && cType == CT_NONE){ // stats

       comeca_leitura_stats();

        msg->n_times_size = stats->n_times_SIZE;
        msg->n_times_del = stats->n_times_DEL;
        msg->n_times_get = stats->n_times_GET;
        msg->n_times_put = stats->n_times_PUT;
        msg->n_times_getkeys = stats->n_times_GETKEYS;
        msg->n_times_print = stats->n_times_PRINT;

        msg->avg_time = stats->time;

        if(stats->time < 0){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
        }
        else{
            msg->opcode = OP_STATS + 1;
            msg->c_type = CT_RESULT;
        }

        termina_leitura_stats();

    }
    }
    else{

        MessageT__Opcode opCode = msg->opcode;
    MessageT__CType cType =  msg->c_type;
    int resultado, retvalue;

    MessageT__Opcode OP_SIZE = MESSAGE__T__OPCODE__OP_SIZE;
    MessageT__Opcode OP_DEL = MESSAGE__T__OPCODE__OP_DEL;
    MessageT__Opcode OP_GET = MESSAGE__T__OPCODE__OP_GET;
    MessageT__Opcode OP_PUT = MESSAGE__T__OPCODE__OP_PUT;
    MessageT__Opcode OP_GETKEYS = MESSAGE__T__OPCODE__OP_GETKEYS;
    MessageT__Opcode OP_PRINT = MESSAGE__T__OPCODE__OP_PRINT;
    MessageT__Opcode OP_STATS =  MESSAGE__T__OPCODE__OP_STATS;
    MessageT__Opcode OP_ERROR = MESSAGE__T__OPCODE__OP_ERROR;

    MessageT__CType CT_KEY = MESSAGE__T__C_TYPE__CT_KEY;
    MessageT__CType CT_VALUE = MESSAGE__T__C_TYPE__CT_VALUE;
    MessageT__CType CT_ENTRY = MESSAGE__T__C_TYPE__CT_ENTRY;
    MessageT__CType CT_KEYS = MESSAGE__T__C_TYPE__CT_KEYS;
    MessageT__CType CT_RESULT = MESSAGE__T__C_TYPE__CT_RESULT;
    MessageT__CType CT_TABLE = MESSAGE__T__C_TYPE__CT_TABLE;
    MessageT__CType CT_NONE = MESSAGE__T__C_TYPE__CT_NONE;

    retvalue = zoo_exists(zh, backup_path, 0, NULL);

    if(opCode == OP_SIZE && cType == CT_NONE){ // size
        msg->opcode = OP_SIZE + 1;
        msg->c_type = CT_RESULT;

        comeca();
        
        resultado = table_size(table);

        termina();

        comeca_leitura_stats();

        int n_times_SIZE = stats->n_times_SIZE;

        termina_leitura_stats();

        n_times_SIZE++;

        comeca_escrita_stats();

        stats->n_times_SIZE = n_times_SIZE;

        termina_escrita_stats();
         
        msg->size = resultado;
    }

    if(opCode == OP_DEL && cType == CT_KEY){ // del <key>

        if(ZOK != retvalue){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }

        int hashCode = hash(msg->key, numberLists);

        comeca_listas(hashCode, 1);

        resultado = table_del(table, msg->key);

        termina_listas(hashCode, 1);

        comeca_leitura_stats();

        int n_times_DEL = stats->n_times_DEL;

        termina_leitura_stats();

        n_times_DEL++;

        comeca_escrita_stats();

        stats->n_times_DEL = n_times_DEL;

        termina_escrita_stats();

        if(resultado == -1){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }
        
        msg->opcode = OP_DEL + 1;
        msg->c_type = CT_NONE;
        
        
    }

    if(opCode == OP_GET && cType == CT_KEY){ // get <key>

        int hashCode = hash(msg->key, numberLists);

        comeca_listas(hashCode, 2);

        struct data_t *data = table_get(table, msg->key);
    
        termina_listas(hashCode, 2);

        comeca_leitura_stats();

        int n_times_GET = stats->n_times_GET;

        termina_leitura_stats();

        n_times_GET++;

        comeca_escrita_stats();

        stats->n_times_GET = n_times_GET;

        termina_escrita_stats();

        if(data == NULL){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }

        msg->opcode = OP_GET + 1;
        msg->c_type = CT_VALUE;
        msg->data.data = data->data;
        msg->data.len = data->datasize;  

    }

    if(opCode == OP_PUT && cType == CT_ENTRY){ // put <key> <data>

        if(ZOK != retvalue){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }

        struct data_t *data;
        
        data = data_create2(msg->data.len, msg->data.data);

        int hashCode = hash(msg->key, numberLists);

        comeca_listas(hashCode, 0); 

        resultado = table_put(table, msg->key, data);

        termina_listas(hashCode, 0);

        comeca_leitura_stats();

        int n_times_PUT = stats->n_times_PUT;

        termina_leitura_stats();

        n_times_PUT++;

        comeca_escrita_stats();

        stats->n_times_PUT = n_times_PUT;

        termina_escrita_stats();

        if(resultado == -1){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
            return -1;
        }
        else{
            msg->opcode = OP_PUT + 1;
            msg->c_type = CT_NONE;
        }

    }

    if(opCode == OP_GETKEYS && cType == CT_NONE){ // getkeys

        msg->opcode = OP_GETKEYS + 1;
        msg->c_type = CT_KEYS;

        comeca_leitura_stats();

        int n_times_GETKEYS = stats->n_times_GETKEYS;

        termina_leitura_stats();

        n_times_GETKEYS++;

        comeca_escrita_stats();

        stats->n_times_GETKEYS = n_times_GETKEYS;

        termina_escrita_stats();

        comeca();
        
        msg->n_info = table_size(table);
        msg->info = table_get_keys(table);

        termina();
    }

    if(opCode == OP_PRINT && cType == CT_NONE){ // table_print

        int i, j;
        char **print; 

        msg->opcode = OP_PRINT + 1;
        msg->c_type = CT_TABLE;

        comeca_leitura_stats();

        int n_times_PRINT = stats->n_times_PRINT;

        termina_leitura_stats();

        n_times_PRINT++;

        comeca_escrita_stats();

        stats->n_times_PRINT = n_times_PRINT;

        termina_escrita_stats();

        j = 0;

        comeca();

        if(table_size(table) != 0){

            print = (char **) malloc(sizeof(char *) * table_size(table));

            for(i = 0; i < table_size(table); i++){
                if(table_get_keys(table)[i] != NULL){
                    char *p = (char *) table_get(table, table_get_keys(table)[i])->data;
                    print[j] = (char *) malloc(snprintf(NULL, 0, "key - %s | data - %s | datasize - %d \n", table_get_keys(table)[i], p, table_get(table, table_get_keys(table)[i])->datasize));
                    sprintf(print[j], "key - %s | data - %s | datasize - %d", table_get_keys(table)[i], p, table_get(table, table_get_keys(table)[i])->datasize);
                    j++;
                }
            }

            msg->n_info = table_size(table);
            msg->info = print; 
        }
        else{
            msg->size = -1;
        }  

        termina(); 
    
    }

    if(opCode == OP_STATS && cType == CT_NONE){ // stats

       comeca_leitura_stats();

        msg->n_times_size = stats->n_times_SIZE;
        msg->n_times_del = stats->n_times_DEL;
        msg->n_times_get = stats->n_times_GET;
        msg->n_times_put = stats->n_times_PUT;
        msg->n_times_getkeys = stats->n_times_GETKEYS;
        msg->n_times_print = stats->n_times_PRINT;

        msg->avg_time = stats->time;

        if(stats->time < 0){
            msg->opcode = OP_ERROR;
            msg->c_type = CT_NONE;
        }
        else{
            msg->opcode = OP_STATS + 1;
            msg->c_type = CT_RESULT;
        }

        termina_leitura_stats();

    }
    }

    return 0;


}

/*
 * Função que atualiza o tempo médio de atendimento a clientes
 */

void update_stats(double time){

    int n_pedidos;
    double tempo_total;

    comeca_leitura_stats();
        
    n_pedidos = 0;
    n_pedidos += stats->n_times_SIZE;
    n_pedidos += stats->n_times_DEL;
    n_pedidos += stats->n_times_GET;
    n_pedidos += stats->n_times_PUT;
    n_pedidos += stats->n_times_GETKEYS;
    n_pedidos += stats->n_times_PRINT;
    tempo_total = (n_pedidos - 1) * stats->time;

    termina_leitura_stats();

    comeca_escrita_stats();

    stats->time = (tempo_total + time) / n_pedidos;

    termina_escrita_stats();
    
}


/*
 * Inicia as stats do servidor
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). 
 */

int stats_init(){

    stats = (struct statistics *) malloc(sizeof(struct statistics));

    if(stats == NULL){
        perror("Erro na criação das stats.\n");
        return -1;
    }

    return 0;
}

/* Liberta toda a memória e recursos alocados pela função stats_init.
 */
void stats_destroy(){

    free(stats);
}
