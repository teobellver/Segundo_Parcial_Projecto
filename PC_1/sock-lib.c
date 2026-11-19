#include "sock-lib.h"

void *ControlPlaca(void *arg)
{
    struct DatosCompartidos *datos = (struct DatosCompartidos *)arg;
    char buffer[1024];
    ssize_t recibido;
    float temp;
    int instruccion;

    while (1)
    {
        recibido = recv(datos->nu_socketplaca, buffer, sizeof(buffer) - 1, 0);
        if (recibido <= 0)
            break;

        buffer[recibido] = '\0';
        temp = strtof(buffer, NULL);

        instruccion = (temp >= 30.0f) ? 1 : 0;

        if (send(datos->nu_socketplaca, &instruccion, sizeof(instruccion), 0) <= 0)
            break;

        pthread_mutex_lock(&datos->mutex);
        datos->temperatura = temp;
        datos->nueva_temperatura = 1;
        pthread_cond_signal(&datos->cond_temp);
        pthread_mutex_unlock(&datos->mutex);
    }

    pthread_mutex_lock(&datos->mutex);
    datos->activo = 0;
    pthread_cond_broadcast(&datos->cond_temp);
    pthread_mutex_unlock(&datos->mutex);

    return NULL;
}

void *ControlPC2(void *arg)
{
    struct DatosCompartidos *datos = (struct DatosCompartidos *)arg;
    float temp;

    while (1)
    {
        pthread_mutex_lock(&datos->mutex);

        while (datos->nueva_temperatura == 0 && datos->activo)
        {
            pthread_cond_wait(&datos->cond_temp, &datos->mutex);
        }

        if (!datos->activo)
        {
            pthread_mutex_unlock(&datos->mutex);
            break;
        }

        temp = datos->temperatura;
        datos->nueva_temperatura = 0;
        pthread_mutex_unlock(&datos->mutex);

        if (send(datos->nu_socketpc2, &temp, sizeof(float), 0) <= 0)
            break;
    }

    pthread_mutex_lock(&datos->mutex);
    datos->activo = 0;
    pthread_cond_broadcast(&datos->cond_temp);
    pthread_mutex_unlock(&datos->mutex);

    shutdown(datos->nu_socketplaca, SHUT_RDWR);

    return NULL;
}

int AperturaDelSocket(int port)
{
    int socket_fd;
    struct sockaddr_in mi_direccion;
    int resu;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        perror("Error de socket");
        return -1;
    }

    mi_direccion.sin_family = AF_INET;
    mi_direccion.sin_port = htons(port);
    mi_direccion.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(mi_direccion.sin_zero), '\0', 8);

    resu = bind(socket_fd, (struct sockaddr *)&mi_direccion, sizeof(struct sockaddr));
    if (resu == -1)
    {
        perror("Error de bind");
        return -1;
    }

    resu = listen(socket_fd, BACKLOG);
    if (resu == -1)
    {
        perror("Error de listen");
        return -1;
    }
    return socket_fd;
}

int AceptarConexion(int socket_fd, struct DatosCompartidos *datos, uint8_t select)
{
    int nu_socket;
    struct sockaddr_in tu_direccion;
    socklen_t largo_struct = sizeof(struct sockaddr_in);

    nu_socket = accept(socket_fd, (struct sockaddr *)&tu_direccion, &largo_struct);
    if (nu_socket == -1)
    {
        perror("Error de accept");
        return -1;
    }

    if (select == PLACA)
    {
        strcpy(datos->direccionplaca, inet_ntoa(tu_direccion.sin_addr));
        datos->nu_socketplaca = nu_socket;
    }
    else if (select == PC2)
    {
        strcpy(datos->direccionpc2, inet_ntoa(tu_direccion.sin_addr));
        datos->nu_socketpc2 = nu_socket;
    }

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
    memset(&(their_addr.sin_zero), '\0', 8);

    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Error tratando de conectar al server");
        exit(1);
    }
    return sockfd;
}