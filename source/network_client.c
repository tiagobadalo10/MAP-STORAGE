/*
Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

*/


#include "client_stub.h"
#include "network_client.h"
#include "sdmessage.pb-c.h"
#include "client_stub-private.h"
#include "message.h"
#include "inet.h"



/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) a base da
 *   informação guardada na estrutura rtable;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 *   na estrutura rtable;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtable_t *rtable){

    if(rtable == NULL){
        perror("A rtable é NULL");
        return -1;
    }

    int sockfd, res; 

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1){
        perror("Erro ao criar socket TCP");
        free(rtable->serverhostname);
        return -1;
    }

    res = inet_pton(AF_INET, rtable->serverhostname, &rtable->server.sin_addr);

    if(res == -1){
        perror("Error no inet_pton");
        free(rtable->serverhostname);
        close(sockfd);
        return -1;
    }

    rtable->server.sin_family = AF_INET;

    res = connect(sockfd, (const struct sockaddr *) &rtable->server, sizeof(rtable->server));

    if(res == -1){
        perror("Erro ao connectar-se ao servidor");
        free(rtable->serverhostname);
        close(sockfd);
        return -1;
    }

    rtable->sockfd = sockfd;

    return 0;
}

    

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtable_t;
 * - Reservar memória para serializar a mensagem contida em msg;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Libertar a memória ocupada pela mensagem serializada enviada;
 * - Esperar a resposta do servidor;
 * - Reservar a memória para a mensagem serializada recebida;
 * - De-serializar a mensagem de resposta, reservando a memória 
 *   necessária para a estrutura MessageT que é devolvida;
 * - Libertar a memória ocupada pela mensagem serializada recebida;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
struct MessageT *network_send_receive(struct rtable_t * rtable, struct MessageT *msg){

    int socket, nbytes;
    unsigned len;
    uint8_t *buf;

    socket = rtable->sockfd;
    
    len = message__t__get_packed_size(msg);

    buf = (uint8_t *) malloc(len); 

    if(buf == NULL){
        perror("Erro na criação do buf.\n");
        return NULL;
    }

    message__t__pack(msg, (uint8_t *) buf);

    len = htonl(len);

    nbytes = write_all(socket, &len, sizeof(int)); 

    if(nbytes == -1){
        perror("Erro na escrita.\n");
        close(socket);
        return NULL;
    }    

    nbytes = write_all(socket, buf, ntohl(len));

    if(nbytes == -1){
        perror("Erro na escrita.\n");
        close(socket);
        return NULL;
    }

    free(buf);

    unsigned tamanhomensagem;

    nbytes = read_all(socket, &tamanhomensagem, sizeof(int));

    if(nbytes == -1){
        perror("Erro na leitura.\n");
        close(socket);
        return NULL;
    }

    tamanhomensagem = ntohl(tamanhomensagem);

    buf = (uint8_t *) malloc(tamanhomensagem);

    if(buf == NULL){
        perror("Erro a criar o buf.\n");
        return NULL;
    }

    nbytes = read_all(socket, buf, tamanhomensagem); 

    if(nbytes == -1){
        perror("Erro na leitura.\n");
        close(socket);
        return NULL;
    }

    msg = message__t__unpack(NULL, tamanhomensagem, buf);

    free(buf);
    
    return msg;

}

/* A função network_close() fecha a ligação estabelecida por
 * network_connect().
 */
int network_close(struct rtable_t * rtable){

    int res;

    if(rtable == NULL){
        perror("A rtable é NULL.\n");
        return -1;
    }

    res = close(rtable->sockfd);

    if(res == -1){
        perror("Erro a fechar o file descriptor.\n");
        return -1;
    }

    return res;
}