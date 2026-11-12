#include <pthread.h>

#include "AtencionDelCliente.h"
#include "lib.h"
#define MAXDATASIZE     100 	// máximo número de bytes que se pueden leer de una vez

extern pthread_mutex_t mutex;

void * AtencionDeUnCliente ( void * d )
{
	struct DatosDelCliente *datos;
	datos = (struct DatosDelCliente *) d;
    char aux[MAXDATASIZE] ;
    int numbytes;

	strcpy( aux , "Se contactaron desde : " );
	strcat( aux , datos->direccion );
	strcat( aux , "\n" );

	pthread_mutex_lock( &mutex );
	puts(aux);

	// ¡Te escucho!
	if ( write( datos->nu_socket , "Contame algo!\n" ,  strlen("Contame algo!\n") ) == -1 )
		perror("send");


	if ( ( numbytes = read(datos->nu_socket, aux, MAXDATASIZE-1) ) == -1 )
	{
        perror("read");
        exit(1);
	}
    aux[numbytes] = '\0';
    printf( "Recivido: %s" , aux );

	pthread_mutex_unlock( &mutex );
	close( datos->nu_socket );
	sleep ( 10 );
	puts("chau!\n");
	return 0;
}
