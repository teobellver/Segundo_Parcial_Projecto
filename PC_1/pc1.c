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

void *HiloPlaca(void *arg)
{
    struct EstadoCompartido *estado = (struct EstadoCompartido *)arg;
    char buffer[128];
    ssize_t recibido;

    while (1)
    {
        pthread_mutex_lock(&estado->mutex);
        int sigue_activo = estado->activo;
        int sock_placa = estado->sock_placa;
        pthread_mutex_unlock(&estado->mutex);

        if (!sigue_activo || sock_placa < 0)
            break;

        recibido = recv(sock_placa, buffer, sizeof(buffer) - 1, 0);
        if (recibido <= 0)
        {
            printf("Placa desconectada o error en recv.\n");
            break;
        }

        buffer[recibido] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (strlen(buffer) == 0)
            continue;

        char *endptr = NULL;
        float temp = strtof(buffer, &endptr);
        if (endptr == buffer)
        {
            fprintf(stderr, "Dato no numérico recibido de la placa: '%s'\n", buffer);
            continue;
        }

        // Instrucción opcional hacia la placa según temperatura (ej: >= 30.0 -> 1, sino 0)
        int instruccion = (temp >= 30.0f) ? 1 : 0;
        send(sock_placa, &instruccion, sizeof(instruccion), MSG_NOSIGNAL);

        pthread_mutex_lock(&estado->mutex);
        estado->temperatura = temp;
        estado->nueva_temperatura = 1;
        pthread_cond_signal(&estado->cond_temp);
        pthread_mutex_unlock(&estado->mutex);

        printf("[PC1] Recibido de placa: %.2f °C\n", temp);
    }

    pthread_mutex_lock(&estado->mutex);
    estado->activo = 0;
    pthread_cond_broadcast(&estado->cond_temp);
    pthread_mutex_unlock(&estado->mutex);

    return NULL;
}

void *HiloPC2(void *arg)
{
    struct EstadoCompartido *estado = (struct EstadoCompartido *)arg;

    while (1)
    {
        pthread_mutex_lock(&estado->mutex);

        while (estado->activo && !estado->nueva_temperatura)
        {
            pthread_cond_wait(&estado->cond_temp, &estado->mutex);
        }

        if (!estado->activo)
        {
            pthread_mutex_unlock(&estado->mutex);
            break;
        }

        float temp_a_enviar = estado->temperatura;
        int sock_pc2 = estado->sock_pc2;
        estado->nueva_temperatura = 0;
        pthread_mutex_unlock(&estado->mutex);

        if (sock_pc2 < 0)
            break;

        size_t enviados = 0;
        const unsigned char *ptr = (const unsigned char *)&temp_a_enviar;

        while (enviados < sizeof(float))
        {
            ssize_t s = send(sock_pc2, ptr + enviados, sizeof(float) - enviados, 0);
            if (s <= 0)
            {
                perror("Error enviando a PC2");
                pthread_mutex_lock(&estado->mutex);
                estado->activo = 0;
                pthread_cond_broadcast(&estado->cond_temp);
                pthread_mutex_unlock(&estado->mutex);
                return NULL;
            }
            enviados += (size_t)s;
        }

        printf("[PC1] Enviado a PC2: %.2f °C\n", temp_a_enviar);
    }

    pthread_mutex_lock(&estado->mutex);
    estado->activo = 0;
    pthread_cond_broadcast(&estado->cond_temp);
    pthread_mutex_unlock(&estado->mutex);

    if (estado->sock_placa >= 0)
        shutdown(estado->sock_placa, SHUT_RDWR);

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

        if (pthread_create(&th_placa, NULL, HiloPlaca, &estado) != 0)
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

        if (pthread_create(&th_pc2, NULL, HiloPC2, &estado) != 0)
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