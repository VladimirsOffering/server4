#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>
#include "Logger.h"
#include "Game.h"
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

in_addr ip_to_num;
const UINT16 TCP_PORT = 12345;
const UINT16 UDP_PORT = 12346;
int Game::count = 0;
Game GameSession;


void UDPServer();

int StartServer()
{
	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ServSock == INVALID_SOCKET) {

		Logger::errorLog("Ошибка инициализации TCP сокета");
		closesocket(ServSock);
		WSACleanup();
		return -1;
	}

	Logger::log("Иницализация сокета TCP прошла успешно");

	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr.s_addr = INADDR_ANY;
	servInfo.sin_port = htons(TCP_PORT);

	if (bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo)) != 0) {
		Logger::errorLog("Ошибка биндинга TCP сокета, возможно порт уже занят");
		closesocket(ServSock);
		WSACleanup();
		return -1;
	}

	Logger::log("Успешный биндинг сокета TCP");

	if (listen(ServSock, SOMAXCONN) != 0) {
		Logger::errorLog("Ошибка ожидания подключений TCP клиентов");
		closesocket(ServSock);
		WSACleanup();
		return -1;
	}

	Logger::log("Ожидание подключений TCP клиентов...");

	// Обработчик запросов клиентов на обнаружение серверов
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)UDPServer, NULL, NULL, NULL);



	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));
	int clientInfo_size = sizeof(clientInfo);

	SOCKET ClientConnection;



	while (1)
	{
		ClientConnection = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);
		if (ClientConnection == INVALID_SOCKET) {
			Logger::errorLog("Ошибка при подключении TCP клиента");
			closesocket(ClientConnection);
			WSACleanup();
			continue;
		}
		User* user = new User(GameSession, ClientConnection);
	}

	closesocket(ServSock);
	WSACleanup();
	return 0;
}




void UDPServer()
{
	SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, NULL);
	if (udpSocket == INVALID_SOCKET) {
		Logger::errorLog("Ошибка UDP сокета, обнаружение сервера не работает");
		WSACleanup();
		exit(1);
		return;
	}

	Logger::log("Иницализация сокета UDP прошла успешно");

	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(UDP_PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(udpSocket, (LPSOCKADDR)&server, sizeof(server)) != 0) {
		Logger::errorLog("Ошибка биндинга UDP сокета, , обнаружение сервера не работает");
		closesocket(udpSocket);
		WSACleanup();
		exit(1);
		return;
	}
	Logger::log("Успешный биндинг сокета UDP");

	sockaddr_in client;
	int clientLength = sizeof(client);
	ZeroMemory(&client, clientLength);
	char buffer[5];
	while (1)
	{
		ZeroMemory(buffer, 5);
		int bytesReceive = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (sockaddr*)&client, &clientLength);

		//Ошибка, если клиент/сервер отключился
		if (bytesReceive == SOCKET_ERROR) {
			Logger::errorLog("Ошибка получения данных от клиента(UDP)");
			continue;
		}

		char clientIP[256];
		ZeroMemory(clientIP, 256);
		inet_ntop(AF_INET, &client.sin_addr, clientIP, 256);
		Logger::log("Получено широковещательное сообщение от " + string(clientIP));
		string message = to_string(GameSession.getCurrentCountPlayers()) + "|" + to_string(GameSession.getCountMaxPlayers());
		int bytesSend = sendto(udpSocket, message.c_str(), message.length(), 0, (sockaddr*)&client, clientLength);
		if (bytesSend == SOCKET_ERROR) {
			Logger::errorLog("Ошибка отправки данных клиенту(UDP)");
		}
	}
}


int main()
{
	setlocale(LC_ALL, "rus");
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
		Logger::errorLog("Ошибка инициализации WinSock");
		return -1;
	}
	StartServer();
	return 0;

}
