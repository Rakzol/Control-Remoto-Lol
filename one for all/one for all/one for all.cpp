//Excluye la librerira winsock1.0 y otras, de la libreria windows.h,
//que se agregan por default sin esta linea
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

// q w e r d a f 1 2 3 4 5 6 7 f2 click_izquierdo, click_derecho
int teclas_banderas[17] = { 'q', 'w', 'e', 'r', 'd', 'f', 'a', '1', '2', '3', '4', '5', '6', '7', 't', 'n', 'm' };
int teclas_identificador[17] = { 0x51, 0x57, 0x45, 0x52, 0x44, 0x46, 0x41, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x71, 0x01, 0x02 };
// No seria necesario almacenar el estado de la tecla si a la hora de leer el estado de la tecla presionada,
// te marcara algo distinto la primera vez que la presiones y algo distinto si la lees y ya la lesite presionada
// lo cual si pasa cuando la lees la primera vez levantada y la segunda que siempre son returns distintos
char teclas_estados[17] = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };
char tecla_estado;
char tecla_envio[2];

int ancho = GetSystemMetrics(0);
int alto = GetSystemMetrics(1);
POINT cursor = {0}, cursor_actual;
char porcentaje[7] = {0};
char cursor_bandera[1];

SOCKET clientes[4];

void mensaje(char* datos, int longitud) {
	for (int conexion = 0; conexion < 4; conexion++) {
		send(clientes[conexion], datos, longitud, 0);
	}
}

void ver_teclas() {
	for (int tecla = 0; tecla < 17; tecla++) {
		tecla_estado = GetAsyncKeyState(teclas_identificador[tecla]) < 0 ? '1' : '0';
		if (tecla_estado != teclas_estados[tecla]) {
			teclas_estados[tecla] = tecla_estado;
			tecla_envio[0] = teclas_banderas[tecla];
			tecla_envio[1] = tecla_estado;
			mensaje(tecla_envio, 2);
		}
	}
}

void ver_cursor() {
	GetCursorPos(&cursor_actual);
	if (cursor_actual.x != cursor.x) {
		cursor.x = cursor_actual.x;
		snprintf(porcentaje, 8, "%f", (float)100 * cursor.x / ancho);
		cursor_bandera[0] = 'x';
		mensaje(cursor_bandera, 1);
		mensaje(porcentaje, 7);
	}
	if (cursor_actual.y != cursor.y) {
		cursor.y = cursor_actual.y;
		snprintf(porcentaje, 8, "%f", (float)100 * cursor.y / alto);
		cursor_bandera[0] = 'y';
		mensaje(cursor_bandera, 1);
		mensaje(porcentaje, 7);
	}
	//if (cursor_actual.x != cursor.x || cursor_actual.y != cursor.y) {
	//	cursor = cursor_actual;
	//	mensaje(cursor_bandera, 1);
	//	snprintf(porcentaje, 8, "%f", (float)100 * cursor.x / ancho);
	//	mensaje(porcentaje, 7);
	//	snprintf(porcentaje, 8, "%f", (float)100 * cursor.y / alto);
	//	mensaje(porcentaje, 7);
	//}
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET servidor;
	int conexiones = 0;
	struct addrinfo* resolucion = NULL;
	struct addrinfo opciones = {0};

	// Se inicializa la libreria
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Error de inicializacion: %d", WSAGetLastError());
		return 1;
	}

	opciones.ai_family = AF_INET;
	opciones.ai_socktype = SOCK_STREAM;
	opciones.ai_protocol = IPPROTO_TCP;
	opciones.ai_flags = AI_PASSIVE;

	// Obtenemos la IPv4 de la maquina local
	if (getaddrinfo(NULL, argv[1], &opciones, &resolucion) != 0) {
		printf("Error de inicializacion: %d", WSAGetLastError());
		return 1;
	}

	// Se crea el socket del serivdor
	if ((servidor = socket(resolucion->ai_family, resolucion->ai_socktype, resolucion->ai_protocol)) == -1)
	{
		printf("No se pudo crear el socket del servidor : %d", WSAGetLastError());
		return 1;
	}

	// Bindeamos el socket con la direccion IP y puerto
	if (bind(servidor, resolucion->ai_addr, (int)resolucion->ai_addrlen) == -1)
	{
		printf("Error al bindear el servidor : %d", WSAGetLastError());
		return 1;
	}

	freeaddrinfo(resolucion);

	// Nos ponemos a escuchar conexiones, con la cola maxima que permita el sistema
	// (SO = sistema opeativo, MAX = maximo, CONN = conexiones)
	listen(servidor, SOMAXCONN);

	printf("Esperando conexiones 0 / 4 . . .\n");
	while (conexiones < 1) {

		if ( (clientes[conexiones] = accept(servidor, NULL, NULL) ) == -1)
		{
			printf("Error al aceptar el socket : %d", WSAGetLastError());
			return 1;
		}

		//tecla_envio[0] = 'p';
		//tecla_envio[1] = conexiones;
		//send(clientes[conexiones], tecla_envio, 2, 0);

		printf("Cliente conectado %i / 4\n", conexiones + 1);
		conexiones++;
	}

	printf("Conexiones establecidas, replicando movimientos\n");
	while (1) {
		ver_teclas();
		ver_cursor();
	}

	return 0;
}
