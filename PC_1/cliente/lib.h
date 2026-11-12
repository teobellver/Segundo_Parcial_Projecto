#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>


#define BACKLOG     10
struct DatosDelCliente
{
	char direccion[ 30 ];
	int nu_socket;
};

int AperturaDelSocket( int port );

int AceptarConexion ( int socket_fd , struct DatosDelCliente *datos );




	int ConectarAlServidor ( char * host , int port );




