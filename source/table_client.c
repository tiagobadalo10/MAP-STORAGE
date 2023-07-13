/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "client_stub.h"
#include "client_stub-private.h" 
#include "table_skel-private.h"
#include "table_skel.h"
#include "table_client.h"
#include "table_client-private.h"
#include "inet.h"
#include "stats-private.h"
#include "signal.h"
#include <stdbool.h>
#include "zookeeper/zookeeper.h"
#include "errno.h"

#define SIG_IGN ((__sighandler_t) 1)
#define ZDATALEN 1024 * 1024

static zhandle_t *zh;
static int connected;

static char* zoo_root = "/kvstore"; 
static char *watcher_ctx = "ZooKeeper Data Watcher";
char *primary_path = "/kvstore/primary";

char *primary_host_ip;
struct rtable_t *rtable;
int boolean;

/* Função que dá watch da mudança de estado de conexão
 *
 */
void connection_watcher_client(zhandle_t *zzh, int type, int state, const char *path, void *context){

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
static void child_watcher_client(zhandle_t *wzh, int type, int state, const char *zpath, void *watcher_ctx){

    zoo_string* children_list = (zoo_string *) malloc(sizeof(zoo_string));
    int retvalue, size;

    size = ZDATALEN;

    if(children_list == NULL){
        perror("Erro na criação da children list.\n");
        exit(EXIT_FAILURE);
    }

    if(state == ZOO_CONNECTED_STATE){

        if(type == ZOO_CHILD_EVENT){

            if(ZOK != zoo_wget_children(zh, zoo_root, child_watcher_client, watcher_ctx, children_list)){
                perror("Erro a dar watch aos filhos de /kvstore.\n");
                exit(EXIT_FAILURE);
            } 

            retvalue = zoo_exists(zh, primary_path, 0, NULL);

            if(ZNONODE == retvalue){ // Se saiu o servidor primário
                rtable_disconnect(rtable);
                printf("\nNão é possível executar operações de escrita na tabela.\n");
                    
            }

            if(ZOK == retvalue){ // Se entrou o servidor primário
                
                primary_host_ip = (char *) malloc(size);

                if(primary_host_ip == NULL){
                    perror("Erro a criar o primary_host_ip.\n");
                    exit(EXIT_FAILURE);
                }

                retvalue = zoo_get(zh, primary_path, 0, primary_host_ip, &size, NULL);

                if(ZOK != retvalue){
                    perror("Erro a dar get do /kvstore/primary.\n");
                    exit(EXIT_FAILURE);
                }

                rtable = rtable_connect(primary_host_ip);

                if(rtable == NULL){
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    
}

int main(int argc, char **argv){

    if(argc != 2){
        printf("Uso: table_client <IP>:<porta> \n"); // <IP>:<porta> do zookeeper
        printf("Exemplo: table_client 127.0.0.1:4000 \n");
        exit(0);
    }

    if(verify_input(argc, argv) == -1){
        printf("Parâmetros inválidos \n");
        printf("Uso: table-client <IP>:<porta> \n");
        printf("Exemplo: table_client 127.0.0.1:4000 \n");
        exit(0);
    }

    printf("Comandos disponíveis:\n");
    printf("| put <key> <data>  |\n");
    printf("| get <key>         |\n");
    printf("| del <key>         |\n");
    printf("| size              |\n");
    printf("| getkeys           |\n");
    printf("| table_print       |\n");
    printf("| stats             |\n");
    printf("| quit              |\n");
    printf("| help              |\n\n");

    char comando[MAX_MSG];
    int code, res, i, retvalue, size;

    size = ZDATALEN;

    zh = zookeeper_init(argv[1], connection_watcher_client, 2000, 0, 0, 0); // Ligar ao ZooKeeper 

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

    retvalue = zoo_wget_children(zh, zoo_root, &child_watcher_client, watcher_ctx , children_list);

    if (ZOK != retvalue) { // Obter e fazer watch aos filhos de /kvstore
        perror("Erro a dar watch aos filhos de /kvstore.\n");
        return -1;
    }

    if(connected){
        
        retvalue = zoo_exists(zh, primary_path, 0, NULL);

        if(ZNONODE == retvalue){ // terminar o cliente
            perror("Não existe /kvstore/primary.\n");
            return 0;
        }

        if(ZOK == retvalue){
            
            primary_host_ip = (char *) malloc(size);

            if(primary_host_ip == NULL){
                perror("Erro a criar o buff.\n");
                return -1;
            }

            retvalue = zoo_get(zh, primary_path, 0, primary_host_ip, &size, NULL);

            if(ZOK != retvalue){
                perror("Erro a dar get dos metadados do /kvstore/primary.\n");
                return -1;
            }

            rtable = rtable_connect(primary_host_ip);

            if(rtable == NULL){
                perror("Erro a conectar ao servidor.\n");
                return -1;
            }

            struct data_t *data;
            struct entry_t *entry;
            char *p, *t;
            struct statistics *stats;

            printf("Insira um novo comando:\n");
            fgets(comando, MAX_MSG, stdin);

            if (comando == NULL){
                return -1;
            }

            comando[strcspn(comando, "\n")] = 0;        

            code = get_code(comando);

            signal(SIGPIPE, SIG_IGN);

            while(true){

                switch(code){

                    case 1:
                        res = rtable_size(rtable);
                        printf("O número de elementos contidos na tabela é %d.\n\n", res);
                        break;
                    case 2:
                        strtok(comando, " ");
                        char *token = strtok(NULL, " ");
                        p = strdup(token); 
                        res = rtable_del(rtable, p);
                        if(res == 0){
                            printf("O elemento foi removido com sucesso.\n\n");
                        }
                        else{
                            printf("A chave não foi encontrada ou aconteceu algum problema na remoção do elemento.\n\n");
                        }
                        break;
                    case 3:
                        strtok(comando, " ");
                        char *token1 = strtok(NULL, " ");
                        p = strdup(token1);
                        data = rtable_get(rtable, p);
                        if(data != NULL){
                            if(data->data == NULL && data->datasize == 0){
                                printf("A chave não foi encontrada.\n\n");    
                            }
                            else{
                                printf("datasize = %d \ndata = ", data->datasize);
                                char *p = (char *) data->data;
                                printf("%s\n\n", p);
                            }      
                            free(data);
                        }         
                        free(p);
                        break;
                    case 4:
                        strtok(comando, " ");
                        char *token2 = strtok(NULL, " ");
                        p = strdup(token2);
                        char *token3 = strtok(NULL, " ");
                        t = strdup(token3);
                        data = data_create2(strlen(t) + 1, t);
                        entry = entry_create(p , data);
                        res = rtable_put(rtable, entry);
                        if(res == 0){
                            printf("O elemento foi adicionado ou substituido.\n\n");
                        }
                        else{
                            printf("Ocorreu um problema na adição do elemento.\n\n");
                        }
                        entry_destroy(entry);
                        break;
                    case 5:
                        i = 0;
                        char **p = rtable_get_keys(rtable);
                        if(p == NULL){
                            printf("Tabela vazia.\n\n");
                            break;
                        }
                        while(p[i] != NULL){
                            printf("key - %s\n", p[i]);
                            i++;
                        }
                        printf("\n");
                        break;
                    case 6:
                        rtable_print(rtable);
                        printf("\n");
                        break;
                    case 7:
                        stats = rtable_stats(rtable);
                        printf("Número de vezes que a operação SIZE foi executada - %d\n", stats->n_times_SIZE);
                        printf("Número de vezes que a operação DEL foi executada - %d\n", stats->n_times_DEL);
                        printf("Número de vezes que a operação GET foi executada - %d\n", stats->n_times_GET);
                        printf("Número de vezes que a operação PUT foi executada - %d\n", stats->n_times_PUT);
                        printf("Número de vezes que a operação GETKEYS foi executada - %d\n", stats->n_times_GETKEYS);
                        printf("Número de vezes que a operação TABLE_PRINT foi executada - %d\n", stats->n_times_PRINT);
                        printf("Tempo médio do atendimento do pedido -  %lf\n", stats->time);
                        printf("\n");
                        free(stats);
                        break;
                    case 8:
                        printf("put <key> <data> - Adiciona uma entry à tabela.\n");
                        printf("get <key> - Recebe uma entry da tabela dado a key.\n");
                        printf("del <key> - Remove uma entry da tabela dado a key.\n");
                        printf("size - Retorna o número de elementos contidos na tabela.\n");
                        printf("getkeys - Retorna as keys da tabela.\n");
                        printf("table_print - Display da tabela.\n");
                        printf("stats - Display da estatísticas.\n");
                        printf("quit - Termina a execução.\n");
                        printf("\n");
                        break;
                    case 9:
                        rtable_disconnect(rtable);
                        return 0;
                    default:
                        printf("Comando não reconhecido \n\n");
                        break;
                    
                }

                printf("Insira um novo comando:\n");
                fgets(comando, MAX_MSG, stdin);

                if (comando == NULL){
                    return -1;
                }

                comando[strcspn(comando, "\n")] = 0;        

                code = get_code(comando);   

            }

            res = rtable_disconnect(rtable);

            if(res == -1){
                perror("Ocorreu um erro a terminar a conexão.\n\n");
                return -1;
            }
        }
    }
    
    return 0;
}


   