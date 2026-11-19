#include "sock-lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PORT        3468
#define MAXDATASIZE 128

void registrar_temperatura(float temperatura)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char hora[20];
    snprintf(hora, sizeof(hora), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);

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
    ssize_t numbytes;
    char buf[MAXDATASIZE];

    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <IP_de_PC1>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 192.168.1.50\n", argv[0]);
        exit(1);
    }

    // Asegurar que datos.csv tenga cabecera si se crea por primera vez
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
        numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
        if (numbytes < 0)
        {
            perror("Error en recv");
            break;
        }
        if (numbytes == 0)
        {
            printf("PC1 cerró la conexión.\n");
            break;
        }

        buf[numbytes] = '\0';
        buf[strcspn(buf, "\r\n")] = '\0';

        if (strlen(buf) == 0)
            continue;

        // Si PC1 envía varias lecturas pegadas en el mismo recv (ej: "22.50\n22.60\n"),
        // separamos línea por línea para no perder ninguna
        char *linea = strtok(buf, "\r\n");
        while (linea != NULL)
        {
            char *endptr = NULL;
            float temp = strtof(linea, &endptr);

            if (endptr != linea)
            {
                registrar_temperatura(temp);
                printf("[RECIBIDO DE PC1] %.2f °C -> guardado en datos.csv\n", temp);
            }
            else
            {
                printf("[MENSAJE DE PC1] %s\n", linea);
            }

            linea = strtok(NULL, "\r\n");
        }
    }

    close(sockfd);
    return 0;
}