#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

#include <netinet/in.h>
#include "zookeeper/zookeeper.h"

struct rtable_t {
    struct sockaddr_in server;
    int sockfd;
    char *serverhostname;
};

/* Função que dá watch da mudança de estado de conexão
 *
 */
void connection_watcher(zhandle_t *zzh, int type, int state, const char *path, void *context);

#endif
