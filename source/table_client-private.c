/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "inet.h"
#include "table_client-private.h"

/*
 * Devolve o code corresponde ao comando colocado pelo utilizador. Se não existir, devolve -1.
 */

int get_code(char* comando){

    char *p = strdup(comando);

    if(p == NULL){
        perror("Erro a duplicar uma string.\n");
        return -1;
    }

    int code;
    char *token;

    token = strtok(p, " ");

    if(strcmp(token, "size") == 0){
        code = 1;
    }
    else if(strcmp(token, "del") == 0){
        code = 2;
    }
    else if(strcmp(token, "get") == 0){
        code = 3;
    }
    else if(strcmp(token, "put") == 0){
        code = 4;
    }
    else if(strcmp(token, "getkeys") == 0){
        code = 5;
    }
    else if(strcmp(token, "table_print") == 0){
        code = 6;   
    }
    else if(strcmp(token, "stats") == 0){
        code = 7;
    }
    else if(strcmp(token, "help") == 0){
        code = 8;
    }
    else if(strcmp(token, "quit") == 0){
        code = 9;
    }
    
    free(p);

    return code;

}

/*
 * Função que verifica se os argumentos são inteiros e válidos
 */

int verify_input(int argc, char **argv){

    char *p = (char *) malloc(strlen(argv[1]) + 1);

    if(p == NULL){
        perror("Erro a alocar memória.\n");
        return -1;
    }

    strcpy(p, argv[1]);

    char *token1 = strtok(p, ":");

    char *token6 = strtok(NULL, ":");

    if(atoi(token6) < 1023 || atoi(token6) > 65535){
        return -1;
    }

    int i;

    for(i = 0; i < 4; i++){
        
        char *token2 = strtok(token1, ".");

        if(atoi(token2) < 0 || atoi(token2) > 255){
            return -1;
        }

    } 

    free(p);

    return 0;
}
