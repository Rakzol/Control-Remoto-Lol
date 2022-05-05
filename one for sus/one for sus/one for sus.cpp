#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib,"Ws2_32.lib") //Winsock Library

SOCKET servidor;

INPUT input = { 0 };
char dato[1];

void teclear(int tecla){
	recv(servidor, dato, 1, 0);
	if (dato[0] == '1') {
		input.ki.dwFlags = 0;
	}
	else {
		input.ki.dwFlags = 2;
	}
	input.ki.wScan = tecla;
	SendInput(1, &input, 40);
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	struct addrinfo* resolucion = NULL;
	struct addrinfo opciones = {0};
	
	int ancho = GetSystemMetrics(0)-1;
	int alto = GetSystemMetrics(1)-1;
	float ancho_calc = (float)65536 / ancho;
	float alto_calc = (float)65536 / alto;
	char porcentaje[7];

	POINT cursor;
	INPUT puntero = {0};
	puntero.type = INPUT_MOUSE;
	puntero.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
	float porcentaje_dec;

	INPUT click = { 0 };
	click.type = INPUT_MOUSE;

	input.type = INPUT_KEYBOARD;

	// Se inicializa la libreria
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Error de inicializacion: %d", WSAGetLastError());
		return 1;
	}

	// Crando el socket del cliente
	opciones.ai_family = AF_UNSPEC;
	opciones.ai_socktype = SOCK_STREAM;
	opciones.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(argv[1], argv[2], &opciones, &resolucion) != 0) {
		printf("Error al intentar resolver la direccion del servidor");
		return 1;
	}

	if ((servidor = socket(resolucion->ai_family, resolucion->ai_socktype, resolucion->ai_protocol)) == -1)
	{
		printf("Error al crear el socket del cliente : %d", WSAGetLastError());
		return 1;
	}

	// Conectando con el servidor
	if (connect(servidor, resolucion->ai_addr, (int)resolucion->ai_addrlen) == -1)
	{
		printf("Error al conectar con el servidor : %d", WSAGetLastError());
		return 1;
	}

	freeaddrinfo(resolucion);

	printf("Esperando datos. . .\n");
	while (1) {
		recv(servidor, dato, 1, 0);
		//if(dato[0] == 'x') {
		//	recv(servidor, porcentaje, 7, 0);
		//	sscanf_s(porcentaje, "%f", &porcentaje_dec);
		//	puntero.mi.dx = (int)(porcentaje_dec * 0.01 * ancho * ancho_calc);
		//	recv(servidor, porcentaje, 7, 0);
		//	sscanf_s(porcentaje, "%f", &porcentaje_dec);
		//	puntero.mi.dy = (int)(porcentaje_dec * 0.01 * alto * alto_calc);
		//	SendInput(1, &puntero, 40);
		//}
		if(dato[0] == 'x' || dato[0] == 'y') {
			GetCursorPos(&cursor);
			recv(servidor, porcentaje, 7, 0);
			sscanf_s(porcentaje, "%f", &porcentaje_dec);
			if (dato[0] == 'x') {
				puntero.mi.dx = (int)(porcentaje_dec * 0.01 * ancho * ancho_calc);
				puntero.mi.dy = (int)(alto_calc * cursor.y);
			}
			else {
				puntero.mi.dx = (int)(ancho_calc * cursor.x);
				puntero.mi.dy = (int)(porcentaje_dec * 0.01 * alto * alto_calc);
			}
			SendInput(1, &puntero, 40);
		}
		else if (dato[0] == 'n') {
			recv(servidor, dato, 1, 0);
			if (dato[0] == '1') {
				click.mi.dwFlags = 2;
			}
			else {
				click.mi.dwFlags = 4;
			}
			SendInput(1, &click, 40);
		}
		else if (dato[0] == 'm') {
			recv(servidor, dato, 1, 0);
			if (dato[0] == '1') {
				click.mi.dwFlags = 8;
			}
			else {
				click.mi.dwFlags = 16;
			}
			SendInput(1, &click, 40);
		}
		else{
			switch (dato[0]) {
				case 'q':
					teclear(0x10);
					break;
				case 'w':
					teclear(0x11);
					break;
				case 'e':
					teclear(0x12);
					break;
				case 'r':
					teclear(0x13);
					break;
				case 'd':
					teclear(0x20);
					break;
				case 'f':
					teclear(0x21);
					break;
				case 'a':
					teclear(0x1E);
					break;
				case '1':
					teclear(0x02);
					break;
				case '2':
					teclear(0x03);
					break;
				case '3':
					teclear(0x04);
					break;
				case '4':
					teclear(0x05);
					break;
				case '5':
					teclear(0x06);
					break;
				case '6':
					teclear(0x07);
					break;
				case '7':
					teclear(0x08);
					break;
				case 't':
					recv(servidor, dato, 1, 0);
					input.ki.dwFlags = 0;
					input.ki.wScan = 0x3C;
					SendInput(1, &input, 40);
					break;
			}
		}
	}

	return 0;
}
