/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/


#include "message-private.h"
#include "table_skel-private.h"
#include "inet.h"

#define ZDATALEN 1024 * 1024

typedef struct String_vector zoo_string; 


/* Função que retorna o <IP:port>
 *  
 */

char *get_host_port(char *port){

    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;

    gethostname(hostbuffer, sizeof(hostbuffer));  
            
    host_entry = gethostbyname(hostbuffer);
               
    IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); 

    strcat(IPbuffer, ":");
    strcat(IPbuffer, port);

    return IPbuffer;
}

