/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/

#include "message.h"
#include "inet.h"
#include <errno.h>


/* Função que envia sequências inteiras de bytes pela rede
 */
int write_all(int sockfd, void *buf, int len){

    int res, bufsize;

    bufsize = len;

    while(len > 0){
        res = write(sockfd, buf, len);
        if(res < 0){
            if(errno == EINTR) continue; 
            perror("Escrita falhada.\n");
            return -1;
        }
        buf += res;
        len -= res;
    }
    
    return bufsize;
}

/* Função que recebe sequências inteiras de bytes pela rede
 */
int read_all(int sockfd, void *buf, int len){

    int res, bufsize;

    bufsize = len;

    while(len > 0){
        res = read(sockfd, buf, len);
        if(res < 0){
            if(errno == EINTR) continue;
            perror("Leitura falhada.\n");
            return -1;
        }
        buf += res;
        len -= res;
    }

    return bufsize;

}