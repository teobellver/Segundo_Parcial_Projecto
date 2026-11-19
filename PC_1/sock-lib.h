#ifndef SOCK_LIB_H
#define SOCK_LIB_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <stdint.h>

#define BACKLOG 10
#define PLACA   0
#define PC2     1

struct DatosDelCliente
{
    char direccion[30];
    int nu_socket;
};

struct DatosCompartidos
{
    char direccionplaca[30];
    char direccionpc2[30];
    int nu_socketplaca;
    int nu_socketpc2;
    pthread_mutex_t mutex;
    pthread_cond_t cond_temp;
    float temperatura;
    int nueva_temperatura;
    int activo;
};

int AperturaDelSocket(int port);
int AceptarConexion(int socket_fd, struct DatosCompartidos *datos, uint8_t select);
int ConectarAlServidor(char *host, int port);

void *ControlPlaca(void *arg);
void *ControlPC2(void *arg);

#endif