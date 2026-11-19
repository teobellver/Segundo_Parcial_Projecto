#include <LPC845.h>
#include <wifi.h>
#include <string.h>
#define RED_WIFI    "TU_RED_WIFI"
#define CLAVE_WIFI  "TU_CONTRASEÑA"
#define IP_DESTINO  "192.168.1.100" // Cambia esto por la IP de tu PC
#define PUERTO      8080
#define PUERTO_STR  "8080"

void Retardo_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < (ms * 3000); i++);
}

// Convierte un número entero a texto sin usar stdio.h
void int_a_texto(int num, char *str) {
    int i = 0;
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }
    str[i] = '\0';
    for (int j = 0; j < i / 2; j++) {
        char tmp = str[j];
        str[j] = str[i - 1 - j];
        str[i - 1 - j] = tmp;
    }
}

int main(void) {
    char post_request[160];
    char cmd_send[32];
    char len_str[8];

    // 1. Inicializa COM_1 a 115200 baudios para el ESP8266[cite: 5]
    Inicializar_WIFI();

    // 2. Conecta al Wi-Fi y al servidor[cite: 5]
    ConnectToServer(RED_WIFI, CLAVE_WIFI, IP_DESTINO, PUERTO);
    Retardo_ms(3000);

    // 3. Arma el HTTP POST con el "22" (Content-Length: 2)
    strcpy(post_request, "POST / HTTP/1.1\r\nHost: ");
    strcat(post_request, IP_DESTINO);
    strcat(post_request, ":");
    strcat(post_request, PUERTO_STR);
    strcat(post_request, "\r\nContent-Type: text/plain\r\nContent-Length: 2\r\n\r\n22");

    // 4. Arma y envía "AT+CIPSEND=<longitud>\r\n"[cite: 5]
    int_a_texto((int)strlen(post_request), len_str);
    strcpy(cmd_send, "AT+CIPSEND=");
    strcat(cmd_send, len_str);
    strcat(cmd_send, "\r\n");

    Transmit_ESP8266(cmd_send);
    Retardo_ms(500);

    // 5. Envía el paquete HTTP[cite: 5]
    Transmit_ESP8266(post_request);

    while (1);
    return 0;
}
