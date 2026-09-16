# REPOSITORIO PARA PRUEBAS DE SEGURIDAD Y CIBERSEGURIDAD EN SUBESTACIONES ELÉCTRICAS

## ÓSCAR ANDRÉS MORENO DÍAZ
## INGENIERÍA ELÉCTRICA
## UNIVERSIDAD NACIONAL DE COLOMBIA

El actual repositorio digital es para registrar el avance en técnicas de ciberseguridad y protección remota de sistemas eléctricos, mecánicos y de telecomunicaciones en subestaciones eléctricas de potencia.

Se registra primera prueba de ciberseguridad, en lenguaje C, para simular un disparo de protección ante falla de subestación. Realizado el miércoles 16 de septiembre de 2026.

# Primera Prueba de Ciberseguridad: Simulación del disparo de una protección ante una falla, estableciendo conexión con almacenamiento por NUBE en territorio extranjero

1. **Inicialización de la Capa de Abstracción en la Red (Winsock API)**
Se le solicita al sistema operativo (WINDOWS 11) cargar la versión 2.2 de la especificación **Windows Sockets API**. Esta llamada asigna las estructuras de datos de memoria para gestionar la pila TCP/IP.
Si la inicialización falla o es alterada mediante una inyección en la memoria del sistema, ell dispositivo piere toda capacidad de comunicación de red, generando una condición de **Denegación de Servicio (DoS)** a nivel de interfaz de comunicaciones.

```c 
WSDATA wsa; 
WSAStratup(MAKEWORD(2, 2), &wsa);
```
2. **Creación del Endpoint de Red (Socket Streaming)**
El socket es la interfaz lógica a través de la cual la aplicación interactúa con la capa de transporte:
```server_fd = socket(AF_INET, SOCK_STREAM, 0);```
- AF_INET: Establece que la comunicación utilizará el esquema de direccionamiento IP de 32 bits.
- SOCK_STREAM: Configura un transporte orientado a conexión (Transmission Control Protocol). Garantiza la entrega ordenada y confiable de paquetes mediante mecanismos de acuse de recibo (ACK) y retransmisión.
- Capa OSI: Opera en la Capa 4 (Transporte).

3. **Asinación de la Interfaz y Enlace de Puerto**
la etapa define las reglas de escucha del dispositivo local dentro de la red:
```c
addres.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);
bind(server_fd, (struct sockaddr *)&address, sizeof(address));
``` 
- INADDR_ANY: Le indica al socjet que escuche repeticiones entrantes en todas las interfaces de red físicas y virtuales disponibles en el equipo.
- hotns(): Convierte el número de puerto del formate de orden de bytes de la arquitectura de la CPU local al formato estándar de red.
- Falla de Seguridad; Al usar INADDR_ANY, la aplicaicón no restringe el orden de los paquetes a una IP esecífica o interfaz privada. Cualquier actor en la red puede alcanzar el puerto.

4. **Establecimineot de la Sesión y Conexión**
implmenta el modelo Servidor en espera pasiva:
```c
listen(server_fd, 3);
new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
```
- listen(server_fd, 3): Pone el socket en modo de escucha y define un backlog de máximo 3 repeticiones antes de rechazarlas.
- accept(): NBloquea la ejecución del programa hasta que un cliente remoto inicia el *Three-Way Handshake*.
Al completar el saludo de tres vías, se crea un nuevo socket dedicado exclusivamente a esa sesión de comunicación.

5. **Recepción y Procesamiento de la Carga Útil**
Capa de aplicación y evaluación del vector de ataque:
```c
recv(new_socket, buffer, 1024, 0);
if(strncmp(buffer, "KILL_SIGNAL", 11) == 0){
    detonar_falla();
}
```
- recv(): Lee los bytes transmitidos por el socket dentro del buffer de memoria local.
**Analítica del Vector de Ataque**:
1. Tráfico en Texto Plano: Los datos viajan sin cifrar. Un sniffer de red intercepta la palabra *KILL_SIGNAL* inmediatamente.
2. Ausencia de Autenticación/Firma Digital: El programa no exige credenciales, certificados X.509 ni una firma HMAC para validar que el emisor realmente tiene autorización operativa.
3. Comprbación Estática: La evaluación se limita a comparar una cadena de caracteres fija. No hay uso de nonces o marcas de tiempo, lo que hace al sistema 100% vulnerable a Ataques de Replay.