#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>

/*********************************************************************************
* IMPOPRTANTE !!!!
* sock-lib.c: Es este archivo vas a poder ver como están armadas las funciones 
 * para establecer una conexion 
 * sock-lib.h: Es el archivo de encabezados. Lo tenés que agregar en tu programa y 
* tiene el encabezado con el uso de cada función.
* ********************************************************************************/

/* 
 * Puertos reservados: 0 a 1023
 * Puertos "libres": 1024 a 65353
 */
 
#define BACKLOG     10
#define PLACA     0
#define PC2     1

typedef unsigned char uint8_t;


struct DatosDelCliente
{
	char direccion[ 30 ];
	int nu_socket;
};


struct DatosCompartidos
{
	char direccionplaca[ 30 ];
	char direccionpc2[ 30 ];
	int nu_socketplaca;
	int nu_socketpc2;
    pthread_mutex_t mutex;
    pthread_cond_t cond_temp;
    float temperatura;
    int nueva_temperatura;
};

/**
	\fn    int AperturaDelSocket( int port );
	\brief  Función que crea la conexión, para usar en el servidor. Establece como puerto de escucha el puerto definido en port y como dirección IP, 
			todas las direcciones IP de la máquina que corre el programa.
	\param  port: numero de puerto
	\return Devuelve el número de file descriptor del socket o -1 si se produjo un error en la creación.
*/
int AperturaDelSocket( int port );

/**
	\fn     int AceptarConexion ( int socket_fd , struct DatosDelCliente *datos );
	\brief  Utilizada por el servidor es una función que acepta una conexión entrante. 
	\param  int: Numero del socket sobre el cual realizar el accept
			struct DatosDelCliente *: puntero a una estructura donde quedaran cargados los datos del cliente
	\return Devuelve el número de file descriptor devuelto por el accept para continuar la comunicación o -1 si se produjo un error en el accept.
*/
int AceptarConexion ( int socket_fd , struct DatosDelCliente *datos );

/**
	\fn     int ConectarAlServidor ( char * host , int port );
	\brief  Función a utilizar en el cliente. Espera recibir como argumento la IP del servidor y  el port.
	\param  char * : direccion del host.
            int: numero de puerto.
	\return Devuelve -1 por error y 0 por exito.
*/
int ConectarAlServidor ( char * host , int port );

