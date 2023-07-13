#ifndef _STATS_PRIVATE_H
#define _STATS_PRIVATE_H


/* Estrutura que define as estatísticas.
 */
struct statistics {
    int n_times_SIZE;      /* Número de vezes que a operação SIZE na tabela foi executada no servidor */
    int n_times_DEL;       /* Número de vezes que a operação DEL na tabela foi executada no servidor */
    int n_times_GET;       /* Número de vezes que a operação GET na tabela foi executada no servidor */
    int n_times_PUT;       /* Número de vezes que a operação PUT na tabela foi executada no servidor */
    int n_times_GETKEYS;   /* Número de vezes que a operação GETKEYS na tabela foi executada no servidor */
    int n_times_PRINT;     /* Número de vezes que a operação PRINT na tabela foi executada no servidor */
    double time;           /* Tempo médio de atendimento de pedidos dos clientes. */
};

#endif