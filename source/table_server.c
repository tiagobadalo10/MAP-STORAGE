/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "table_skel.h"
#include "network_server.h"
#include "table_server-private.h"
#include "table_skel-private.h"
#include "inet.h"
#include "signal.h"


int socket_de_escuta;

void ctrlC(int sig){
    
    int resultado;

    table_skel_destroy();

    stats_destroy();

    resultado = network_server_close(socket_de_escuta);

    if(resultado == -1){
        perror("Erro a fechar o socket");
    }

    exit(0);
}

int main(int argc, char **argv){

    int resultado;

    if(argc != 4){
        printf("Uso: table-server <port> <n_lists>  <IP>:<porta> \n"); // <IP>:<porta> do zookeeper
        printf("Exemplo: table_server 3201 4 127.0.0.1:2181 \n");
        exit(0);
    }

    if(verify_input(argc, argv) == -1){
        printf("Parâmetros inválidos \n");
        printf("Uso: table-server <port> <n_lists> <IP>:<porta> \n");
        printf("Exemplo: table_server 3201 127.0.0.1:2181 \n");
        exit(0);
    }

    signal(SIGINT, ctrlC);
    signal(SIGPIPE, SIG_IGN);

    socket_de_escuta = network_server_init(atoi(argv[1]));

    if(socket_de_escuta == -1){
        perror("Erro na criação do listensock.\n");
        return -1;
    }

    resultado = table_skel_init(atoi(argv[2]), argv[3], argv[1]);

    if(resultado == -1){
        perror("Erro na criação do skeleton da tabela.\n");
        return -1;
    }

    resultado = stats_init();

    if(resultado == -1){
        perror("Erro na criação das stats.\n");
        return -1;
    }

    resultado = network_main_loop(socket_de_escuta);

    if(resultado == -1){
        perror("Erro no network main loop.\n");
        return -1;
    }

    return 0;
    
}

 