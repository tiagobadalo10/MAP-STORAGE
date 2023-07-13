#ifndef _TABLE_CLIENT_H
#define _TABLE_CLIENT_H

#include "client_stub.h"
#include "table_client-private.h"

/* Função que dá watch da mudança de estado de conexão
 *
 */
void connection_watcher_client(zhandle_t *zzh, int type, int state, const char *path, void *context);

#endif

