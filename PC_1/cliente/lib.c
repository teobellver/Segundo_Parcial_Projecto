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
#include "lib.h"

#define BACKLOG     10

// Funcioones del Servidor
int AperturaDelSocket( int port )
{
    int socket_fd;
    struct sockaddr_in mi_direccion;
    int resu;

    //int socket(int domain, int type, int protocol);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("Error de socket: ");
        return -1;
    }

    mi_direccion.sin_family = AF_INET;
    mi_direccion.sin_port = htons(port);
    mi_direccion.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(mi_direccion.sin_zero),'\0',8);

    //int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    resu = bind(socket_fd, (struct sockaddr *) &mi_direccion, sizeof(struct sockaddr) );

    if (resu == -1)
    {
        perror("Error de bind: ");
        return -1;
    }

    //int listen(int sockfd, int backlog);
    resu = listen(socket_fd, BACKLOG);
    if (resu == -1)
    {
        perror("Error de listen: ");
        return -1;
    }
    return socket_fd;
}

int AceptarConexion ( int socket_fd , struct DatosDelCliente *datos )
{
    int nu_socket ;
    struct sockaddr_in tu_direccion;
    unsigned int largo_struct;

    largo_struct = sizeof(struct sockaddr);

    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    nu_socket = accept(socket_fd, (struct sockaddr *) &tu_direccion, &largo_struct);
    strcpy( datos->direccion, inet_ntoa(tu_direccion.sin_addr));
    datos->nu_socket = nu_socket;

    if ( nu_socket == -1)
    {
        perror("Error de accept: ");
        return -1;
    }
    return 0;
}

// Funcion del Cliente
int ConectarAlServidor ( char * h , int port )
{
	int	sockfd , cantidad = 0;
	struct in_addr addr;
	struct hostent *he;	/* Se utiliza para convertir el nombre del host a su dirección IP */
	struct sockaddr_in their_addr;  /* dirección del server donde se conectará */
	char* token,host[30];

	strcpy ( host , h);

	// Usamos strtok para dividir la cadena por el delimitador "."
    token = strtok(h, ".");
	while (token != NULL) 
	{
        // Convertimos cada token (subcadena) a entero o flotante
        cantidad +=  atoi(token) ;
        // Obtenemos el siguiente token
        token = strtok(NULL, ".");
    }

	if ( cantidad )
	{
		if (inet_aton( host , &addr) == 0) 
		{
			perror("Invalid address");
			exit(1);
		}
	}
	else
	{
		/* Convertimos el nombre del host a su dirección IP */
		if ((he = gethostbyname ( (const char *) host )) == NULL)
		{
			perror("Error en gethostbyname");
			exit(1);
		}
	}
	
        /* Creamos el socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Error en creación de socket");
		exit(1);
	}

        /* Establecemos their_addr con la direccion del server */
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	
	if ( cantidad )
		their_addr.sin_addr = addr;
	else
		their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	
	bzero((their_addr.sin_zero), 8);

        /* Intentamos conectarnos con el servidor */
	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Error tratando de conectar al server");
		exit(1);
	}
	return sockfd;
}
