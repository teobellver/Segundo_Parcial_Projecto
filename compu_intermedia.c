#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ip_command "ipconfig | findstr IPv4" //o "hostname -I" en linux 
char ip[64] = { 0 };
char borrador[128];

#define DEBUG_MODE

#ifdef _WIN32 //entra solo si se corre en windows

	// --- LIBRERÍAS Y CONFIGURACIÓN PARA WINDOWS ---
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Exclusivo de Visual Studio


#else //entra solo si se corre en cualquier otra cosa
	// --- LIBRERÍAS Y ALIAS PARA LINUX / MACOS ---
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// En Linux, un socket es un simple 'int'. En Windows es un tipo 'SOCKET'.
// Creamos estos alias para que Linux pueda leer el código de Windows sin errores.
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

// En Windows se usa closesocket(), en Linux se usa close()
#define closesocket close
#endif
// =========================================================================

#define PUERTO_HTTP 8080
#define BUFFER_SIZE 2048






int main() {
	FILE* pipe = _popen(ip_command, "r"); // FILE es una estructura que permite la manipulacion de archivos  para guardar salidas de comandos y imprimirlos con las funciones de impresion en pantalla de archivos
	if (pipe == NULL) {
		perror("Error opening pipe");
		return;
	}
	while (fgets(borrador, sizeof(borrador), pipe) != NULL) {
		char* character = strchr(borrador, ':');
		if (character != NULL) {
			character++; // Avanza después de los dos puntos
			while (*character == ' ' || *character == '\t') character++; // Salta espacios
			strncpy(ip, character, sizeof(ip) - 1);
			ip[strcspn(ip, "\r\n")] = 0; // Elimina salto de línea
			break; // Solo la primera IP encontrada
		}
	}
	_pclose(pipe);
	if (strlen(ip) == 0) {
		printf("Error: No se pudo detectar la IP local.\n");
		return 1;
	}
	#ifdef DEBUG_MODE
	printf("IP Local detectada: %s\n", ip);
	#endif 

	//ahora que tenemos la ip cruda en la variable ip, podemos imprimirla en pantalla o mandarla por serial..

	/// serial port opening


	// Variables para la red
	SOCKET server_fd, client_fd;
	// 1. Declaras la variable y la llenas de ceros por seguridad
	struct sockaddr_in server_addr = { 0 };
	struct sockaddr_in client_addr = { 0 };
	int client_addr_len = sizeof(client_addr);
	char buffer[BUFFER_SIZE];



	#ifdef _WIN32 //devuelta, solo en windows 
	WSADATA wsaData;   //La función WSAStartup inicializa la biblioteca de sockets de Windows y debe ser llamada antes de utilizar cualquier función de red en aplicaciones que utilizan Winsock.
		//WSASStartup devuelve cero en exito
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Error al inicializar Winsock. Codigo: %d\n", WSAGetLastError());
		return 1;
	}
	
#endif
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == INVALID_SOCKET) {
        printf("Error al crear el socket. Codigo: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
	
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; //formato de la ip
	server_addr.sin_port = htons(8080); //htons convierte el numero del puerto a "Little endiand, formato universal para las concexiones de red
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		printf("Error en bind(). El puerto %d podria estar en uso.\n", PUERTO_HTTP);
		closesocket(server_fd);
		WSACleanup();
		return 1;
	}

	if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
		printf("Error al intentar escuchar en el puerto.\n");
		closesocket(server_fd);
		WSACleanup();
		return 1;
	}

	printf("Servidor HTTP Windows iniciado. Escuchando en el puerto %d...\n", PUERTO_HTTP);
	while (1) {
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
		if (client_fd == INVALID_SOCKET) {
			printf("Fallo al aceptar la conexion.\n");
			continue;
		}

		memset(buffer, 0, BUFFER_SIZE);
		int bytes_recibidos = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

		if (bytes_recibidos > 0) {
			char* cuerpo_mensaje = strstr(buffer, "\r\n\r\n");

			if (cuerpo_mensaje != NULL) {
				cuerpo_mensaje += 4;
				printf("\n[NUEVO POST RECIBIDO]\n%s\n", cuerpo_mensaje);
			}

			const char* respuesta_http =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Connection: close\r\n"
				"\r\n"
				"Mensaje procesado correctamente";

			send(client_fd, respuesta_http, (int)strlen(respuesta_http), 0);
		}
		closesocket(client_fd);
	}
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
	}
