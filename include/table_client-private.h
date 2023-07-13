#ifndef _TABLE_CLIENT_PRIVATE_H
#define _TABLE_CLIENT_PRIVATE_H

#include <string.h>

/*
 * Devolve o code corresponde ao comando colocado pelo utilizador. Se não existir, devolve -1.
 */

int get_code(char* comando);

/*
 * Função que verifica se os argumentos são inteiros e válidos
 */

int verify_input(int argc, char **argv);

#endif