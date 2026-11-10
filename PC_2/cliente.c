#include "sock-lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PORT            3468    // Puerto de escucha
#define MAXDATASIZE     100 	// Máximo número de bytes a leer

// Mutex para evitar conflictos al escribir en el archivo CSV desde múltiples hilos
pthread_mutex_t mutex_archivo = PTHREAD_MUTEX_INITIALIZER;

// Función que registra la temperatura recibida en un archivo CSV para que la web lo lea
void registrar_temperatura(float temperatura) {
    pthread_mutex_lock(&mutex_archivo);
    
    // Obtener la hora actual exacta
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char hora[20];
    sprintf(hora, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Guardar en el archivo datos.csv
    FILE *f = fopen("datos.csv", "a");
    if (f != NULL) {
        fprintf(f, "%s,%.2f\n", hora, temperatura);
        fclose(f);
    } else {
        perror("Error al abrir datos.csv");
    }

    pthread_mutex_unlock(&mutex_archivo);
}

// Función ejecutada por cada hilo para atender a un cliente/sensor
void *AtencionDeUnClienteWrapper(void *arg) {
    struct DatosDelCliente *datos = (struct DatosDelCliente *)arg;
    char aux[MAXDATASIZE];
    int numbytes;

    printf("[+] Conexión establecida desde la IP: %s\n", datos->direccion);

    // Enviar saludo o solicitud al sensor si es necesario
    if (write(datos->nu_socket, "Contame la temperatura!\n", strlen("Contame la temperatura!\n")) == -1) {
        perror("Error en write");
    }

    // Leer el valor de temperatura enviado por el sensor
    if ((numbytes = read(datos->nu_socket, aux, MAXDATASIZE - 1)) == -1) {
        perror("Error en read");
        close(datos->nu_socket);
        free(datos);
        return NULL;
    }
    
    aux[numbytes] = '\0';
    // Limpiar saltos de línea sobrantes
    aux[strcspn(aux, "\r\n")] = 0;

    printf("[DATO RECIBIDO] Temperatura: %s °C\n", aux);

    // Convertir el texto recibido a número flotante y guardarlo
    float temperatura = atof(aux);
    registrar_temperatura(temperatura);

    // Confirmar recepción al sensor
    if (write(datos->nu_socket, "Dato recibido correctamente\n", strlen("Dato recibido correctamente\n")) == -1) {
        perror("Error en send");
    }

    close(datos->nu_socket);
    printf("[-] Conexión cerrada con %s\n\n", datos->direccion);
    
    free(datos);
    return NULL;
}

int main() {
    int socket_servidor;
    struct DatosDelCliente cliente;

    // Inicializar el archivo CSV con la cabecera si no existe
    FILE *f = fopen("datos.csv", "r");
    if (f == NULL) {
        f = fopen("datos.csv", "w");
        if (f != NULL) {
            fprintf(f, "Hora,Temperatura\n");
            fclose(f);
        }
    } else {
        fclose(f);
    }

    // Abrir el socket usando tu librería sock-lib
    socket_servidor = AperturaDelSocket(PORT);
    if (socket_servidor == -1) {
        fprintf(stderr, "Error crítico: No se pudo abrir el socket del servidor.\n");
        exit(1);
    }

    printf("==================================================\n");
    printf("   SERVIDOR DE TEMPERATURA EN C INICIADO         \n");
    printf("   Escuchando en el puerto: %d                    \n", PORT);
    printf("==================================================\n\n");

    // Bucle principal del servidor
    while (1) {
        if (AceptarConexion(socket_servidor, &cliente) == -1) {
            continue;
        }

        // Crear memoria dinámica para pasar los datos del cliente al hilo
        struct DatosDelCliente *datos_cliente = malloc(sizeof(struct DatosDelCliente));
        *datos_cliente = cliente;

        // Lanzar un hilo independiente para atender al sensor concurrentemente
        pthread_t hilo;
        if (pthread_create(&hilo, NULL, AtencionDeUnClienteWrapper, (void *)datos_cliente) != 0) {
            perror("Error al crear el hilo");
            free(datos_cliente);
            close(cliente.nu_socket);
        } else {
            pthread_detach(hilo); // Autoliberar recursos del hilo al terminar
        }
    }

    close(socket_servidor);
    return 0;
}