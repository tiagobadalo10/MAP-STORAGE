/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "inet.h"
#include "table_server-private.h"


/*
 * Função que verifica se os argumentos são inteiros e válidos
 */

int verify_input(int argc, char **argv){

    if(atoi(argv[1]) <= 0){
        perror("O port é menor ou igual a 0.\n");
        return -1;
    }

    if(atoi(argv[2]) <= 0){
        perror("O número de listas é menor ou igual a 0.\n");
        return -1;
    }

    char *p = (char *) malloc(strlen(argv[2]) + 1);

    if(p == NULL){
        perror("Erro a alocar memória.\n");
        return -1;
    }

    strcpy(p, argv[3]);

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
