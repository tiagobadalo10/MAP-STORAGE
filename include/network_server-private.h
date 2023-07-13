#ifndef _NETWORK_SERVER_PRIVATE_H
#define _NETWORK_SERVER_PRIVATE_H


/* A função thread_sec é a função que vai ser passada como argumento na
 * criação de uma thread secundária
 */

void *thread_sec(void *params);

#endif