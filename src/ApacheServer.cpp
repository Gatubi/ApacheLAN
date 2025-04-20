#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include "ApacheUtils.h"
#pragma comment(lib, "ws2_32.lib")

SOCKET tcpServerSocket = INVALID_SOCKET;
SOCKET udpServerSocket = INVALID_SOCKET;

void StartApacheServer()
{
    IniciaWinsockUnaVez();

    // Activamos el loop
    serverRunning = true;

    SOCKET tcpServerSocket;
    SOCKET udpServerSocket;

    sockaddr_in tcpAddr, udpAddr;

    // TCP SERVER
    tcpServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpServerSocket == INVALID_SOCKET) {
        std::cout << "[Apache] No se pudo crear el socket TCP" << std::endl;
        return;
    }
    std::cout << "[Apache] Socket TCP creado correctamente!" << std::endl;

    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_addr.s_addr = INADDR_ANY;  // <-- Asegúrate de que sigue siendo este
    tcpAddr.sin_port = htons(7777);

    if (bind(tcpServerSocket, (sockaddr*)&tcpAddr, sizeof(tcpAddr)) == SOCKET_ERROR) {
        std::cout << "[Apache] Error al bindear el socket TCP. Error: " << WSAGetLastError() << std::endl;
        closesocket(tcpServerSocket);
        return;
    }
    std::cout << "[Apache] Bind TCP completado!" << std::endl;

    if (listen(tcpServerSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "[Apache] Error al escuchar en el socket TCP. Error: " << WSAGetLastError() << std::endl;
        closesocket(tcpServerSocket);
        return;
    }
    std::cout << "[Apache] Servidor TCP escuchando en puerto 7777..." << std::endl;

    // UDP SERVER
    udpServerSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpServerSocket == INVALID_SOCKET) {
        std::cout << "[Apache] No se pudo crear el socket UDP. Error: " << WSAGetLastError() << std::endl;
        closesocket(tcpServerSocket);
        return;
    }

    udpAddr.sin_family = AF_INET;
    udpAddr.sin_addr.s_addr = INADDR_ANY;
    udpAddr.sin_port = htons(7778);

    if (bind(udpServerSocket, (sockaddr*)&udpAddr, sizeof(udpAddr)) == SOCKET_ERROR) {
        std::cout << "[Apache] Error al bindear el socket UDP. Error: " << WSAGetLastError() << std::endl;
        closesocket(tcpServerSocket);
        closesocket(udpServerSocket);
        return;
    }

    std::cout << "[Apache] Servidor UDP escuchando en puerto 7778..." << std::endl;

    std::thread tcpAcceptThread([tcpServerSocket]() {
        sockaddr_in clientAddr;
        int clientAddrLen;

        while (serverRunning) {
            clientAddrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(tcpServerSocket, (sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocket != INVALID_SOCKET) {
                std::cout << "[Apache] Nuevo cliente TCP conectado!" << std::endl;

                const char* mensaje = "Bienvenido al servidor TCP de Apache!";
                send(clientSocket, mensaje, strlen(mensaje), 0);

                closesocket(clientSocket);
            }
        }
        });
    tcpAcceptThread.detach();

    std::vector<sockaddr_in> clients;  // Guardamos las direcciones de los clientes

    while (serverRunning) {
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);

        PositionPacket packet;
        int bytesReceived = recvfrom(
            udpServerSocket,
            (char*)&packet,
            sizeof(packet),
            0,
            (sockaddr*)&clientAddr,
            &clientAddrLen
        );

        if (bytesReceived == SOCKET_ERROR) {
            std::cout << "[Apache] Error en recvfrom. Código: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "[Apache] Recibido paquete de IP: " << inet_ntoa(clientAddr.sin_addr)
            << " Puerto: " << ntohs(clientAddr.sin_port)
            << " Bytes recibidos: " << bytesReceived << std::endl;

        if (bytesReceived != sizeof(packet)) {
            std::cout << "[Apache] Paquete con tamaño incorrecto. Ignorando." << std::endl;
            continue;
        }

        // 🔹 Verificar si el cliente ya está registrado
        bool knownClient = false;
        for (const auto& c : clients) {
            if (c.sin_addr.s_addr == clientAddr.sin_addr.s_addr && c.sin_port == clientAddr.sin_port) {
                knownClient = true;
                break;
            }
        }

        // 🔹 Si es un nuevo cliente, lo agregamos
        if (!knownClient) {
            clients.push_back(clientAddr);
            std::cout << "[Apache] Nuevo cliente UDP conectado! Total: " << clients.size() << std::endl;
        }

        // 🔹 Reenviar el paquete a todos los clientes menos al emisor
        for (const auto& c : clients) {
            if (c.sin_addr.s_addr != clientAddr.sin_addr.s_addr || c.sin_port != clientAddr.sin_port) {
                sendto(udpServerSocket, (char*)&packet, sizeof(packet), 0, (sockaddr*)&c, sizeof(c));
                std::cout << "[Apache] Reenviado a IP: " << inet_ntoa(c.sin_addr)
                    << " Puerto: " << ntohs(c.sin_port) << std::endl;
            }
        }
    }


    closesocket(tcpServerSocket);
    closesocket(udpServerSocket);

    SafeLog("[SERVER] Servidor cerrado.");
}

void StopApacheServer()
{
    if (!serverRunning)
    {
        std::cout << "[Apache] El servidor ya está detenido!" << std::endl;
        return;
    }

    // Apaga el bucle del servidor
    serverRunning = false;

    // Aquí se cierran los sockets principales
    if (tcpServerSocket != INVALID_SOCKET)
    {
        closesocket(tcpServerSocket);
        tcpServerSocket = INVALID_SOCKET;
        std::cout << "[Apache] Socket TCP cerrado!" << std::endl;
    }

    if (udpServerSocket != INVALID_SOCKET)
    {
        closesocket(udpServerSocket);
        udpServerSocket = INVALID_SOCKET;
        std::cout << "[Apache] Socket UDP cerrado!" << std::endl;
    }

    std::cout << "[Apache] Servidor detenido correctamente!" << std::endl;
    SafeLog("[SERVER] Servidor detenido correctamente");
}





