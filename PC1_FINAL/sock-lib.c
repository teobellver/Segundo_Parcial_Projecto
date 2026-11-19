#include "sock-lib.h"

int AperturaDelSocket(int port)
{
    int socket_fd;
    struct sockaddr_in mi_direccion;
    int resu;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("Error de socket: ");
        return -1;
    }

    mi_direccion.sin_family = AF_INET;
    mi_direccion.sin_port = htons(port);
    mi_direccion.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(mi_direccion.sin_zero), '\0', 8);

    resu = bind(socket_fd, (struct sockaddr *)&mi_direccion, sizeof(struct sockaddr));
    if (resu == -1)
    {
        perror("Error de bind: ");
        return -1;
    }

    resu = listen(socket_fd, BACKLOG);
    if (resu == -1)
    {
        perror("Error de listen: ");
        return -1;
    }
    return socket_fd;
}

int AceptarConexion(int socket_fd, struct DatosDelCliente *datos)
{
    int nu_socket;
    struct sockaddr_in tu_direccion;
    unsigned int largo_struct;

    largo_struct = sizeof(struct sockaddr);

    nu_socket = accept(socket_fd, (struct sockaddr *)&tu_direccion, &largo_struct);
    if (nu_socket == -1)
    {
        perror("Error de accept: ");
        return -1;
    }

    strcpy(datos->direccion, inet_ntoa(tu_direccion.sin_addr));
    datos->nu_socket = nu_socket;
    return 0;
}

int ConectarAlServidor(char *h, int port)
{
    int sockfd;
    struct hostent *he;
    struct sockaddr_in their_addr;

    if ((he = gethostbyname((const char *)h)) == NULL)
    {
        perror("Error en gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error en creación de socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero((their_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Error tratando de conectar al server");
        exit(1);
    }
    return sockfd;
}