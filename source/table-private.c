/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "table-private.h"

/* Função que transforma uma chave num indice da tabela
 *
 */
int hash(char* key, int n){
    return soma(key) % n;
}

/* Função que soma os valores ASCII dos caracteres de uma string
 *
 */
int soma(char* string){
    int soma = 0;
    int i = 0;
    while (string[i] != '\0'){
        soma += string[i];
        i++;
    }
    return soma;
}

