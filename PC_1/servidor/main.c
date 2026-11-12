#include <pthread.h>
#include "AtencionDelCliente.h"
#include "lib.h"

pthread_mutex_t mutex;

int main (void)
{
    int socket_fd , resu;
    struct DatosDelCliente datos;
    pthread_t thread_id;
    
    resu = pthread_mutex_init( &mutex , NULL);
    if ( resu != 0 )
        return 1;
        
    socket_fd = AperturaDelSocket( 3468 );
    if ( socket_fd == -1 )
        return 1;

    while( 1 ) // main accept() loop
    {
        printf("Esperando cliente\n");

        resu =  AceptarConexion ( socket_fd , &datos );
        if ( resu )
                return -1;

        // Creamos un hilo
        pthread_create(&thread_id , NULL , (void *) AtencionDeUnCliente , &datos );
        pthread_detach(thread_id);
    }
    return 0;
}


