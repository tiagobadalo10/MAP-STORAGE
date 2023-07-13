/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/


#include "table_skel.h"
#include "network_server.h"
#include "client_stub-private.h"
#include "table_skel-private.h"
#include "message-private.h"
#include "network_server-private.h"
#include "inet.h"
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>

int listensockfd;

struct thread_parameters {
	int connsockfd;
};


/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port){

    if(port < 0){
        return -1;
    }

    int res, opt; 
    struct sockaddr_in server;
    
    listensockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listensockfd == -1){
        perror("Erro ao criar socket.\n");
        return -1;
    }

    opt = 1;

    res = setsockopt(listensockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (res == -1){
        perror("Erro para fazer bind a um porto usado anteriormente e registado pelo kernel.\n");
        close(listensockfd);
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    res = bind(listensockfd, (const struct sockaddr *)  &server, sizeof(server));

    if(res == -1){
        perror("Erro a fazer bind.\n");
        close(listensockfd);
        return -1;
    }

    res = listen(listensockfd, 0);

    if(res == -1){
        perror("Erro a aceitar conexões no socket.\n");
        close(listensockfd);
        return -1;
    }

    return listensockfd;

}

/* A função thread_sec é a função que vai ser passada como argumento na
 * criação de uma thread secundária
 */

void *thread_sec(void *params){

    int *res;
    int resultado;
    double dif;
    struct MessageT *msg;
    struct thread_parameters *tp = (struct thread_parameters *) params;

    unsigned int conn = (uintptr_t) tp->connsockfd;

    res = &resultado;

    while((msg = network_receive(conn)) != NULL){

        struct timespec inicio, fim;
        
        resultado = clock_gettime(CLOCK_REALTIME, &inicio); 

        if(resultado == -1){
            perror("Erro a receber o tempo.\n");
            return NULL;
        }

        invoke(msg);

        resultado = network_send(conn, msg);
        
        if(resultado == -1){
            perror("Erro a enviar mensagem.\n");
            close(conn);
            return NULL;
        }
        
        resultado = clock_gettime(CLOCK_REALTIME, &fim); 

        if(resultado == -1){
            perror("Erro a receber o tempo.\n");
            return NULL;
        }

        MessageT__Opcode OP_STATS =  MESSAGE__T__OPCODE__OP_STATS;

        if(msg->opcode != OP_STATS + 1){

            dif = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec);

            dif = dif / 1000000000L;

            update_stats(dif);  
        }
    
    }

    resultado = close(conn);

    if(resultado == -1){
        perror("Erro a fechar o file descriptor.\n");
        resultado = -1;
        return res;
    }

    resultado = 0;   
    return res;
}


/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket){

    struct sockaddr_in client;
    socklen_t size_client = sizeof(struct sockaddr_in);
    int connsockfd;

    while (1){

        pthread_t nova;
        struct thread_parameters thread_p;

        connsockfd = accept(listening_socket,(struct sockaddr *) &client, &size_client);

        thread_p.connsockfd = connsockfd;

        if(pthread_create(&nova, NULL, &thread_sec, (void *) &thread_p) != 0){
		    perror("Thread não criada.\n");
		    return -1;
	    }
    }

    return 0;

}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura MessageT.
 */
struct MessageT *network_receive(int client_socket){  

    struct MessageT *msg;
    int nbytes;
    unsigned len;

    nbytes = read_all(client_socket, &len, sizeof(int));

    if(nbytes == -1){
        perror("Erro na leitura.\n");
        close(client_socket);
        return NULL;
    } 

    len = ntohl(len);

    uint8_t *buf = (uint8_t *) malloc(len);
    
    nbytes = read_all(client_socket, buf, len);

    if(nbytes == -1){
        perror("Erro na leitura.\n");
        close(client_socket);
        return NULL;
    } 

    msg = message__t__unpack(NULL, len, buf);

    free(buf);

    return msg;
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct MessageT *msg){

    int nbytes;
    uint8_t *buf;
    unsigned len;
    
    len = message__t__get_packed_size(msg);

    buf = (uint8_t *) malloc(len);

    message__t__pack(msg, buf);

    len = htonl(len);

    nbytes = write_all(client_socket, &len, sizeof(int));

    if(nbytes == -1){
        perror("Erro na escrita.\n");
        close(client_socket);
        return -1;
    } 

    nbytes = write_all(client_socket, buf, ntohl(len)); 

    if(nbytes == -1){
        perror("Erro na escrita.\n");
        close(client_socket);
        return -1;
    } 

    free(buf);

    return 0;
}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init(), nomeadamente fechando o listening socket.
 */
int network_server_close(int listening_socket){

    return close(listening_socket);
}
 