#include "sock-lib.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PUERTO 50000

struct EstadoCompartido
{
    int sock_placa;
    int sock_pc2;
    pthread_mutex_t mutex;
    pthread_cond_t cond_temp;
    float temperatura;
    int nueva_temperatura;
    int activo;
};

void *ControlPlaca(void *arg)
{
    struct EstadoCompartido *datos = (struct EstadoCompartido *)arg;
    char buffer[1024];
    ssize_t recibido;
    float temp;
    int instruccion;

    while (1)
    {
        recibido = recv(datos->sock_placa, buffer, sizeof(buffer) - 1, 0);
        if (recibido <= 0)
            break;

        buffer[recibido] = '\0';
        temp = strtof(buffer, NULL);

        if (temp >= 30.0f)
            instruccion = 1;
        else
            instruccion = 0;

        if (send(datos->sock_placa, &instruccion, sizeof(instruccion), 0) <= 0)
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
    struct EstadoCompartido *datos = (struct EstadoCompartido *)arg;
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

        if (send(datos->sock_pc2X, &temp, sizeof(float), 0) <= 0)
            break;
    }

    pthread_mutex_lock(&datos->mutex);
    datos->activo = 0;
    pthread_cond_broadcast(&datos->cond_temp);
    pthread_mutex_unlock(&datos->mutex);

    shutdown(datos->sock_placa, SHUT_RDWR);

    return NULL;
}

int main(void)
{
    int socket_fd;
    pthread_t th_placa, th_pc2;

    signal(SIGPIPE, SIG_IGN);

    socket_fd = AperturaDelSocket(PUERTO);
    if (socket_fd == -1)
    {
        fprintf(stderr, "No se pudo abrir el socket en el puerto %d\n", PUERTO);
        return 1;
    }

    printf("Servidor PC1 escuchando en el puerto %d...\n", PUERTO);

    while (1)
    {
        struct DatosDelCliente cli_placa = {0};
        struct DatosDelCliente cli_pc2 = {0};

        printf("Esperando conexión de la placa...\n");
        if (AceptarConexion(socket_fd, &cli_placa) == -1)
            continue;
        printf("Placa conectada desde %s\n", cli_placa.direccion);

        printf("Esperando conexión de PC2...\n");
        if (AceptarConexion(socket_fd, &cli_pc2) == -1)
        {
            close(cli_placa.nu_socket);
            continue;
        }
        printf("PC2 conectada desde %s\n", cli_pc2.direccion);

        struct EstadoCompartido estado;
        estado.sock_placa = cli_placa.nu_socket;
        estado.sock_pc2 = cli_pc2.nu_socket;
        estado.temperatura = 0.0f;
        estado.nueva_temperatura = 0;
        estado.activo = 1;

        if (pthread_mutex_init(&estado.mutex, NULL) != 0)
        {
            perror("Error mutex_init");
            close(cli_placa.nu_socket);
            close(cli_pc2.nu_socket);
            continue;
        }

        if (pthread_cond_init(&estado.cond_temp, NULL) != 0)
        {
            perror("Error cond_init");
            pthread_mutex_destroy(&estado.mutex);
            close(cli_placa.nu_socket);
            close(cli_pc2.nu_socket);
            continue;
        }

        if (pthread_create(&th_placa, NULL, ControlPlaca, &estado) != 0)
        {
            perror("Error pthread_create placa");
            pthread_mutex_lock(&estado.mutex);
            estado.activo = 0;
            pthread_mutex_unlock(&estado.mutex);
            pthread_cond_destroy(&estado.cond_temp);
            pthread_mutex_destroy(&estado.mutex);
            close(cli_placa.nu_socket);
            close(cli_pc2.nu_socket);
            continue;
        }

        if (pthread_create(&th_pc2, NULL, ControlPC2, &estado) != 0)
        {
            perror("Error pthread_create pc2");
            pthread_mutex_lock(&estado.mutex);
            estado.activo = 0;
            pthread_cond_broadcast(&estado.cond_temp);
            pthread_mutex_unlock(&estado.mutex);
            shutdown(cli_placa.nu_socket, SHUT_RDWR);
            pthread_join(th_placa, NULL);
            pthread_cond_destroy(&estado.cond_temp);
            pthread_mutex_destroy(&estado.mutex);
            close(cli_placa.nu_socket);
            close(cli_pc2.nu_socket);
            continue;
        }

        pthread_join(th_placa, NULL);
        pthread_join(th_pc2, NULL);

        close(cli_placa.nu_socket);
        close(cli_pc2.nu_socket);

        pthread_cond_destroy(&estado.cond_temp);
        pthread_mutex_destroy(&estado.mutex);

        printf("Sesión finalizada. Esperando nuevos clientes...\n\n");
    }

    close(socket_fd);
    return 0;
}