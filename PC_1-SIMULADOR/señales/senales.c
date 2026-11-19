#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile int salir = 1;

void handle_sigint(int sig);
void handle_SIGUSR1(int sig);
void handle_sigterm(int sig);

int main() 
{
    // Asociamos la señal SIGINT con el manejador handle_sigint
    signal( SIGINT, handle_sigint);

    // Asociamos la señal SIGUSR1 con el manejador handle_SIGUSR1
    signal( SIGUSR1 , handle_SIGUSR1 );

    // Asociamos la señal SIGTERM con el manejador handle_sigterm
    signal( SIGTERM, handle_sigterm);

    printf("Proceso en ejecución. PID: %d\n", getpid());
    printf("Presiona Ctrl+C para enviar SIGINT, \n o usa 'kill -SIGUSR1 %d' en otra terminal para terminar.\n o usa 'kill -SIGTERM %d' en otra terminal para terminar.\n", getpid(),getpid());

    // Bucle infinito para mantener el programa en ejecución
    while (salir) 
	{
        printf("Trabajando... presiona Ctrl+C para SIGINT - SIGTERM o SIGTERM para terminar.\n");
        sleep(3); // Espera 3 segundos
    }

    return 0;
}

// Función para manejar la señal SIGINT
void handle_sigint(int sig)
{
    printf("Señal SIGINT (%d) recibida. Presionaste Ctrl+C.\n", sig);

}

// Función para manejar la señal SIGTERM
void handle_SIGUSR1(int sig)
{
	salir = 0;
}

// Función para manejar la señal SIGTERM
void handle_sigterm(int sig) {
    printf("Señal SIGTERM (%d) recibida. Terminando el programa de forma segura.\n", sig);
    exit(0); // Termina el programa
}
