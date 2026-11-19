#include "sock-lib.h"

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

void *ControlPlaca(void *arg)
{
    struct DatosCompartidos *datos = arg;
    char buffer[1024];
    int recibido;
    float temp;
    int instruccion;

    while (1)
    {
        recibido = recv(datos->nu_socketplaca,
                        buffer, sizeof(buffer) - 1, 0);

        if (recibido <= 0)
            break;

        buffer[recibido] = '\0';
        temp = strtof(buffer, NULL);

        // Evaluamos la temperatura
        if (temp >= 30)
            instruccion = 1;
        else
            instruccion = 0;

        // Enviamos instrucción a la placa
        if (send(datos->nu_socketplaca, &instruccion,
                 sizeof(instruccion), 0) <= 0)
            break;

        // Guardamos la temperatura para PC2
        pthread_mutex_lock(&datos->mutex);

        datos->temperatura = temp;
        datos->nueva_temperatura = 1;

        pthread_cond_signal(&datos->cond_temp); // despierta al hilo que fue dormido con cond_wait

        pthread_mutex_unlock(&datos->mutex);
    }

    // Avisamos que terminó la comunicación
    pthread_mutex_lock(&datos->mutex);

    datos->activo = 0;
    pthread_cond_broadcast(&datos->cond_temp);

    pthread_mutex_unlock(&datos->mutex);

    return NULL;
}

void *ControlPC2(void *arg)
{
    struct DatosCompartidos *datos = arg;
    float temp;

    while (1)
    {
        pthread_mutex_lock(&datos->mutex);

        // Esperamos una temperatura nueva
        while (datos->nueva_temperatura == 0 && datos->activo)
        {
            pthread_cond_wait(&datos->cond_temp, &datos->mutex); // libera el mutex y duerme al hilo
        }

        if (!datos->activo)
        {
            pthread_mutex_unlock(&datos->mutex);
            break;
        }

        temp = datos->temperatura;
        datos->nueva_temperatura = 0;

        pthread_mutex_unlock(&datos->mutex);

        // Enviamos temperatura a PC2
        if (send(datos->nu_socketpc2, &temp,
                 sizeof(float), 0) <= 0)
            break;
    }

    // Avisamos que terminó la comunicación
    pthread_mutex_lock(&datos->mutex);

    datos->activo = 0;
    pthread_cond_broadcast(&datos->cond_temp);

    pthread_mutex_unlock(&datos->mutex);

    // Desbloqueamos a ControlPlaca si está en recv()
    shutdown(datos->nu_socketplaca, SHUT_RDWR);

    return NULL;
}

// Funcioones del Servidor
int AperturaDelSocket(int port)
{
    int socket_fd;
    struct sockaddr_in mi_direccion;
    int resu;

    // int socket(int domain, int type, int protocol);
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

    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    resu = bind(socket_fd, (struct sockaddr *)&mi_direccion, sizeof(struct sockaddr));

    if (resu == -1)
    {
        perror("Error de bind: ");
        return -1;
    }

    // int listen(int sockfd, int backlog);
    resu = listen(socket_fd, BACKLOG);
    if (resu == -1)
    {
        perror("Error de listen: ");
        return -1;
    }
    return socket_fd;
}

int AceptarConexion(int socket_fd, struct DatosCompartidos *datos, uint8_t select)
{
    int nu_socket;
    struct sockaddr_in tu_direccion;
    unsigned int largo_struct;

    largo_struct = sizeof(struct sockaddr);

    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    nu_socket = accept(socket_fd, (struct sockaddr *)&tu_direccion, &largo_struct);
    if (nu_socket == -1)
    {
        perror("Error de accept: ");
        return -1;
    }
    if (select == PLACA)
    {
        strcpy(datos->direccionplaca, inet_ntoa(tu_direccion.sin_addr));
        datos->nu_socketplaca = nu_socket;
    }
    if (select == PC2)
    {
        strcpy(datos->direccionpc2, inet_ntoa(tu_direccion.sin_addr));
        datos->nu_socketpc2 = nu_socket;
    }

    return 0;
}
