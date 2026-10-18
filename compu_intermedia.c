#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ip_command "ipconfig | findstr IPv4" //o "hostname -I" en linux 
char ip[64] = { 0 };
char buffer[128];
void main() {
	FILE* pipe = _popen(ip_command, "r"); // FILE es una estructura que permite la manipulacion de archivos  para guardar salidas de comandos y imprimirlos con las funciones de impresion en pantalla de archivos
	if (pipe == NULL) {
		perror("Error opening pipe");
		return;
	}
	while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
		printf("%s", buffer);
	}
	strncpy(ip, buffer, sizeof(ip) - 1);
	ip[strcspn(ip, "\n")] = 0;
	ip[strcspn(ip, "\r")] = 0;
	_pclose(pipe);
	if (strlen(ip) == 0) {
		printf("Error: No se pudo detectar la IP local.\n");
		return 1;
	}
	printf("IP Local detectada: %s\n", ip);


	system("pause");
	strncpy(ip, buffer, sizeof(ip) - 1);
}
