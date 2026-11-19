#include "sock-lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PORT 3468

void registrar_temperatura(float temperatura)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char hora[20];
    snprintf(hora, sizeof(hora), "%02d:%02d:%02d",
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

    FILE *f = fopen("datos.csv", "a");
    if (f != NULL)
    {
        fprintf(f, "%s,%.2f\n", hora, temperatura);
        fclose(f);
    }
    else
    {
        perror("Error al abrir datos.csv");
    }
}

int main(int argc, char *argv[])
{
    int sockfd;
    float temp_recibida;

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <IP_de_PC1>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 192.168.1.50\n", argv[0]);
        exit(1);
    }

    FILE *f = fopen("datos.csv", "r");
    if (f == NULL)
    {
        f = fopen("datos.csv", "w");
        if (f != NULL)
        {
            fprintf(f, "Hora,Temperatura\n");
            fclose(f);
        }
    }
    else
    {
        fclose(f);
    }

    printf("Conectando a PC1 (%s) en el puerto %d...\n", argv[1], PORT);
    sockfd = ConectarAlServidor(argv[1], PORT);
    printf("Conectado a PC1. Recibiendo temperaturas en tiempo real...\n\n");

    while (1)
    {
        size_t leidos = 0;
        unsigned char *ptr = (unsigned char *)&temp_recibida;

        while (leidos < sizeof(float))
        {
            ssize_t n = recv(sockfd, ptr + leidos, sizeof(float) - leidos, 0);
            if (n < 0)
            {
                perror("Error en recv");
                close(sockfd);
                return 1;
            }
            if (n == 0)
            {
                printf("PC1 cerró la conexión.\n");
                close(sockfd);
                return 0;
            }
            leidos += (size_t)n;
        }

        registrar_temperatura(temp_recibida);
        printf("[RECIBIDO DE PC1] %.2f °C -> guardado en datos.csv\n", temp_recibida);
    }

    close(sockfd);
    return 0;
}