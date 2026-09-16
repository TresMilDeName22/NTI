/*Prueba Inicial de Ciberseguridad en Subestaciones Eléctricas,
Óscar Andrés Moreno Díaz,
Septiembre 16, 2026*/

/*Simulación de una señal de disparo en caso de falla*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<winsock2.h>
#include<ws2tcpip.h>

#define PORT 9500 /*Puerto de conexión con la nube*/

void detonar_falla(){
    printf("\n [!] ALERTA CRÍTICA: Señal de falla recibida.\n");
    printf("[!] Deshabilitando protocolos de seguridad\n");
    printf("[!] Sistema comprometido.\n\n");
}

int main(){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024]= {0};

    /*Creación del socket del servidor*/
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Fallo al crear el socket");
        exit(EXIT_FAILURE);
    }
    /*Configuración de puerto y dirección*/
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //Nube extranjera
    address.sin_port = htons(PORT);

    //Vinculación del socket al puerto 9000
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR){
        printf("Fallo en el bind. Código de error en Winsock: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    //Escucha de conexiones entrantes
    if (listen(server_fd, 3)<0){
        perror("Fallo al escuchar");
        exit(EXIT_FAILURE);
    }
    printf("[*] Dispositivo local activo. Esperando comandos externos en el puerto %d\n", PORT);

    if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) <0){
        perror("Fallo al aceptar la conexión");
        exit(EXIT_FAILURE);
    }
    //Lectura de mensaje enviado por la nube
    read(new_socket, buffer, 1024);
    printf("Mensaje recibido desde la nube: %s\n", buffer);

        //Evaluación si es la señal de disparo
    if(strncmp(buffer, "KILL_SIGNAL", 11) == 0){
        detonar_falla();
    } else{
        printf("[*] Tráfico normal, no se detectan anomalías.\n");
    }

    //Cierre de la conexión
    close(new_socket);
    close(server_fd);
    return 0;
}