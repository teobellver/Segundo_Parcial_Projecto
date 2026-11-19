#include "sock-lib.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int socket_fd;
    struct DatosCompartidos datos = {0};

    pthread_t thread_placa, thread_pc2;

    datos.nu_socketplaca = -1;
    datos.nu_socketpc2 = -1;

    if (pthread_mutex_init(&datos.mutex, NULL) != 0)
        return 1;

    if (pthread_cond_init(&datos.cond_temp, NULL) != 0)
        return 1;

    signal(SIGPIPE, SIG_IGN);

    socket_fd = AperturaDelSocket(3468);
    if (socket_fd == -1)
        return 1;

    while (1)
    {
        printf("Esperando placa\n");
        if (AceptarConexion(socket_fd, &datos, PLACA) != 0)
            continue;

        printf("Placa conectada\n");
        printf("Esperando PC2\n");
        if (AceptarConexion(socket_fd, &datos, PC2) != 0)
        {
            close(datos.nu_socketplaca);
            datos.nu_socketplaca = -1;
            continue;
        }

        printf("PC2 conectada\n");

        pthread_mutex_lock(&datos.mutex);
        datos.nueva_temperatura = 0;
        datos.activo = 1;
        pthread_mutex_unlock(&datos.mutex);

        if (pthread_create(&thread_placa, NULL, ControlPlaca, &datos) != 0)
        {
            close(datos.nu_socketplaca);
            close(datos.nu_socketpc2);
            datos.nu_socketplaca = -1;
            datos.nu_socketpc2 = -1;
            continue;
        }

        if (pthread_create(&thread_pc2, NULL, ControlPC2, &datos) != 0)
        {
            pthread_mutex_lock(&datos.mutex);
            datos.activo = 0;
            pthread_cond_broadcast(&datos.cond_temp);
            pthread_mutex_unlock(&datos.mutex);

            shutdown(datos.nu_socketplaca, SHUT_RDWR);
            pthread_join(thread_placa, NULL);

            close(datos.nu_socketplaca);
            close(datos.nu_socketpc2);
            datos.nu_socketplaca = -1;
            datos.nu_socketpc2 = -1;
            continue;
        }

        pthread_join(thread_placa, NULL);
        pthread_join(thread_pc2, NULL);

        close(datos.nu_socketplaca);
        close(datos.nu_socketpc2);
        datos.nu_socketplaca = -1;
        datos.nu_socketpc2 = -1;

        printf("Clientes desconectados\n");
    }

    pthread_cond_destroy(&datos.cond_temp);
    pthread_mutex_destroy(&datos.mutex);
    close(socket_fd);
    return 0;
}