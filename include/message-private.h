#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#include "sdmessage.pb-c.h"


/* Função que envia sequências inteiras de bytes pela rede
 */
int write_all(int sockfd, void *buf, int len);

/* Função que recebe sequências inteiras de bytes pela rede
 */
int read_all(int sockfd, void *buf, int len);


#endif