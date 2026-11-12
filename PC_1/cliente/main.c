/*
** client.c -- Ejemplo de cliente de sockets de flujo
*/

#include "lib.h"

#define PORT            3468    // Puerto de destino
#define MAXDATASIZE     100 	// máximo número de bytes que se pueden leer de una vez

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];

    if (argc != 2)
    {
        fprintf(stderr,"usar: ./cliente ipdestino\n");
        exit(1);
    }

    sockfd = ConectarAlServidor ( argv[1] , PORT );

    if ( ( numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0) ) == -1 )
    {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
    printf( "Recivido: %s" , buf );
    
    if ( write( sockfd , "Gracias!\n" ,  strlen("Gracias!\n") ) == -1 )
        perror("send");

    
    close(sockfd);

    return 0;
}
