#include "sock-lib.h"
#include <pthread.h>
#include "AtencionDelCliente.h"
#include "lib.h"

int main(void)
{
    int socket_fd, resu;
    struct DatosCompartidos datos;
    pthread_t thread_placa, thread_pc2;

    resu = pthread_mutex_init(&datos.mutex, NULL);
    if (resu != 0)
        return 1;

    socket_fd = AperturaDelSocket(3468);
    if (socket_fd == -1)
        return 1;

    while (1) // main accept() loop
    {
        printf("Esperando cliente\n");

        resu = AceptarConexion(socket_fd, &datos, PLACA);
        resu = AceptarConexion(socket_fd, &datos, PC2);
        if (resu)
            return -1;



        /*
        Esto funciona únicamente si garantizás que la primera conexión corresponde a la placa y la segunda a PC2.

        Una solución más robusta es que cada cliente se identifique al conectarse, por ejemplo enviando PLACA\n o PC2\n.
        */

        // Creamos un hilo
        pthread_create(&thread_placa, NULL, ControlPlaca, &datos);
        pthread_create(&thread_pc2, NULL, ControlPC2, &datos);

        pthread_detach(thread_placa);
        pthread_detach(thread_pc2);



    }
    return 0;
}


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

    // Evita que send() termine el programa
    // si un cliente se desconecta.
    signal(SIGPIPE, SIG_IGN);

    socket_fd = AperturaDelSocket(3468);

    if (socket_fd == -1)
        return 1;

    while (1)
    {
        printf("Esperando placa\n");

        if (AceptarConexion(socket_fd, &datos, PLACA) != 0) //control de errores
            continue;

        printf("Placa conectada\n");
        printf("Esperando PC2\n");

        if (AceptarConexion(socket_fd, &datos, PC2) != 0) //control de errores
        {
            close(datos.nu_socketplaca);
            continue;
        }

        printf("PC2 conectada\n");

        pthread_mutex_lock(&datos.mutex);

        datos.nueva_temperatura = 0;
        datos.activo = 1;

        pthread_mutex_unlock(&datos.mutex);

        // Creamos hilo de la placa
        if (pthread_create(&thread_placa, NULL,
                           ControlPlaca, &datos) != 0)
        {
            close(datos.nu_socketplaca);
            close(datos.nu_socketpc2);
            continue;
        }

        // Creamos hilo de PC2
        if (pthread_create(&thread_pc2, NULL,
                           ControlPC2, &datos) != 0)
        {
            pthread_mutex_lock(&datos.mutex);
            datos.activo = 0;
            pthread_cond_broadcast(&datos.cond_temp);
            pthread_mutex_unlock(&datos.mutex);

            shutdown(datos.nu_socketplaca, SHUT_RDWR);
            pthread_join(thread_placa, NULL);

            close(datos.nu_socketplaca);
            close(datos.nu_socketpc2);
            continue;
        }

        // Esperamos que terminen ambos hilos
        pthread_join(thread_placa, NULL);
        pthread_join(thread_pc2, NULL);

        close(datos.nu_socketplaca);
        close(datos.nu_socketpc2);

        printf("Clientes desconectados\n");
    }

    return 0;
}