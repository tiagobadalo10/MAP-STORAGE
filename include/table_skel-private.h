#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRIVATE_H

#include "table_skel.h"
#include "inet.h"
#include "zookeeper/zookeeper.h"

typedef struct String_vector zoo_string; 

/*
 * Função que atualiza o tempo médio de atendimento a clientes
 */

void update_stats(double time);

/*
 * Inicia as stats do servidor
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). 
 */

int stats_init();

/* Liberta toda a memória e recursos alocados pela função stats_init.
 */
void stats_destroy();

/* Função de controlo de entrada da leitura das stats
 *
 */
void comeca_leitura_stats();

/* Função de controlo de saída da leitura das stats
 *
 */
void termina_leitura_stats();

/* Função de controlo de entrada da escrita das stats
 *
 */
void comeca_escrita_stats();

/* Função de controlo de saída da leitura das stats
 *
 */
void termina_escrita_stats();

/* Função de controlo de entrada nas listas da table
 * Valor do code pode ser 0 (PUT), 1 (DEL), 2 (GET)
 */
void comeca_listas(int hashcode, int code);

/* Função de controlo de saída nas listas da table
 * Valor do code pode ser 0 (PUT), 1 (DEL), 2 (GET)
 */
void termina_listas(int hashcode, int code);

/* Função de controlo de entrada de leitura da table
 *
 */
void comeca();

/* Função de controlo de saída de leitura da table
 *
 */
void termina();

/* Função que dá watch da mudança de estado de conexão
 *
 */
void connection_watcher(zhandle_t *zzh, int type, int state, const char *path, void *context);

/* Função que retorna o <IP:port>
 *  
 */

char *get_host_port(char *port);


#endif