#include "sock-lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define PORT 3468
#define MAXDATASIZE 100

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    char temp_str[20];

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <IP_del_servidor>\n", argv[0]);
        exit(1);
    }

    // Generar una temperatura simulada realista (ej: entre 20.0°C y 35.0°C)
    srand(time(NULL));
    float temperatura_simulada = 20.0 + (float)(rand() % 150) / 10.0;
    sprintf(temp_str, "%.2f", temperatura_simulada);

    // Conectar al servidor usando tu librería
    sockfd = ConectarAlServidor(argv[1], PORT);

    // Recibir el saludo inicial del servidor
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("Servidor dice: %s", buf);

    // Enviar la temperatura medida
    if (write(sockfd, temp_str, strlen(temp_str)) == -1) {
        perror("send");
    }
    printf("Temperatura enviada al servidor: %s °C\n", temp_str);

    // Recibir confirmación
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("Respuesta: %s", buf);

    close(sockfd);
    return 0;
}