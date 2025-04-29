#define WIN32_LEAN_AND_MEAN // Esto le dice al sistema: "No me metas mierdas del siglo pasado"
#include "types.h"
#include <winsock2.h> // Siempre PRIMERO
#include <ws2tcpip.h> // (Opcional pero recomendado para IPv6 y otras funciones)
#include <windows.h>  // Siempre DESPUÉS
#include "ApacheConfig.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <format>
#include <vector>
#include <fstream>
#include "ApacheUtils.h"
#include "ApacheClient.h"
#include <natives.h>

#pragma comment(lib, "ws2_32.lib")

std::unordered_map<int, Actor> remoteActors;
// Este mapa guarda la última posición de cada actor remoto
static std::unordered_map<int, Vector3> lastPositions;

SOCKET udpSocket, tcpSocket;
sockaddr_in udpServer, tcpServer;

// Al inicio del archivo, antes o después de otras funciones auxiliares
void RefreezeDummyAfterTeleport(Actor dummy)
{
    if (!ENTITY::IS_ACTOR_VALID(dummy)) return;

    TASKS::TASK_CLEAR(dummy);
    TASKS::TASK_STAND_STILL(dummy, -1.f, 0, 0);
}


void ReceiveRemotePositions(int listenPort)
{
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    std::cout << "[Apache UDP] Escuchando en puerto local " << listenPort << "..." << std::endl;

    while (clientRunning)
    {
        PositionPacket packet;
        int bytesReceived = recvfrom(
            udpSocket, // 🔹 Usa el mismo socket bindeado en StartApacheClient
            (char*)&packet,
            sizeof(packet),
            0,
            (sockaddr*)&senderAddr,
            &senderAddrSize
        );

        // 🔹 Validación de errores en `recvfrom()`
        if (bytesReceived == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            std::cout << "[Apache UDP] Error en recvfrom(). Código: " << errorCode << std::endl;
            continue;
        }

        // 🔹 Validar que el tamaño del paquete sea correcto
        if (bytesReceived != sizeof(packet))
        {
            std::cout << "[Apache UDP] Tamaño de paquete incorrecto. Recibido: " << bytesReceived << " bytes." << std::endl;
            continue;
        }

        // 🔹 Convertimos a float para asegurarnos de que los valores son válidos
        Vector3 coords = { static_cast<float>(packet.posX), static_cast<float>(packet.posY), static_cast<float>(packet.posZ) };

        // 🔹 Verificación de coordenadas inválidas (NaN, inf, etc.)
        if (!std::isfinite(coords.x) || !std::isfinite(coords.y) || !std::isfinite(coords.z))
        {
            std::cout << "[Apache UDP] Coordenadas inválidas detectadas! Ignorando update de actor " << packet.actorId << std::endl;
            continue;
        }

        // 🔹 Buscar o crear el Dummy
        Actor dummy = FindOrSpawnDummy(packet.actorId, coords);

        if (!ENTITY::IS_ACTOR_VALID(dummy))
        {
            std::cout << "[Apache UDP] Dummy inválido! ActorID: " << packet.actorId << std::endl;
            continue;
        }

        // 🔹 Verificar si el Dummy realmente necesita moverse
        Vector3 currentPos;
        ACTOR::GET_POSITION(dummy, &currentPos);

        float distanceSquared = (coords.x - currentPos.x) * (coords.x - currentPos.x) +
            (coords.y - currentPos.y) * (coords.y - currentPos.y) +
            (coords.z - currentPos.z) * (coords.z - currentPos.z);

        if (distanceSquared > 0.01f) // 🔹 Pequeño umbral para evitar teleports innecesarios
        {
            ACTOR::TELEPORT_ACTOR_WITH_HEADING(
                dummy,
                Vector2(coords.x, coords.y),
                coords.z,
                0.0f,
                false, false, false
            );

            std::cout << "[Apache UDP] Teleport realizado para ActorID " << packet.actorId
                << " a X=" << coords.x << " Y=" << coords.y << " Z=" << coords.z << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 🔹 Pequeño delay para evitar saturar el CPU
    }
}

void SendMyPosition(int actorId)
{
    Actor localActor = ACTOR::GET_PLAYER_ACTOR(ACTOR::GET_LOCAL_SLOT());
    if (!ENTITY::IS_ACTOR_VALID(localActor)) {
        std::cout << "[Apache UDP] ERROR: No se pudo obtener el actor local!" << std::endl;
        return;
    }

    Vector3 coords = { 0.0f, 0.0f, 0.0f };
    ACTOR::GET_POSITION(localActor, &coords);

    // Validar que las coordenadas obtenidas sean finitas
    if (!std::isfinite(coords.x) || !std::isfinite(coords.y) || !std::isfinite(coords.z)) {
        std::cout << "[Apache UDP] ERROR: Coordenadas inválidas del jugador, evitando envío!" << std::endl;
        return;
    }

    PositionPacket packet;
    packet.actorId = actorId;
    packet.posX = coords.x;
    packet.posY = coords.y;
    packet.posZ = coords.z;

    int result = sendto(udpSocket, (char*)&packet, sizeof(packet), 0, (sockaddr*)&udpServer, sizeof(udpServer));

    if (result == SOCKET_ERROR)
    {
        std::cout << "[Apache UDP] Error al enviar posición: " << WSAGetLastError() << std::endl;
    }
    else
    {
        std::cout << "[Apache UDP] Posición enviada: X: " << packet.posX
                  << " Y: " << packet.posY
                  << " Z: " << packet.posZ
                  << " (Jugador " << packet.actorId << ")" << std::endl;
    }
}

void ConnectTCP(std::string ip, int port)
{
    tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket == INVALID_SOCKET)
    {
        std::cout << "[Apache TCP] Error al crear socket TCP!" << std::endl;
        return;
    }

    tcpServer.sin_family = AF_INET;
    tcpServer.sin_port = htons(port);

    int result = inet_pton(AF_INET, ip.c_str(), &tcpServer.sin_addr);
    if (result <= 0)
    {
        std::cout << "[Apache TCP] inet_pton FALLÓ! Resultado: " << result
            << " IP: " << ip << std::endl;
        return;
    }

    std::cout << "[Apache TCP] Intentando conectar a " << ip << ":" << port << "..." << std::endl;

    if (connect(tcpSocket, (sockaddr*)&tcpServer, sizeof(tcpServer)) == SOCKET_ERROR)
    {
        std::cout << "[Apache TCP] Error al conectar! Error: " << WSAGetLastError() << std::endl;
        return;
    }

    std::cout << "[Apache TCP] Conectado al servidor!" << std::endl;
    send(tcpSocket, "Apache conectado", 18, 0);
}

//void SendPosition(Vector3 coords) {
//    std::string message = std::to_string(coords.x) + "," + std::to_string(coords.y) + "," + std::to_string(coords.z);
//    sendto(udpSocket, message.c_str(), message.size(), 0, (sockaddr*)&udpServer, sizeof(udpServer));
//}

void StartUDP(const std::string& ip, int udpSendPort, int udpRecvPort, int actorId)
{
    // Crea el socket UDP
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == INVALID_SOCKET) {
        std::cout << "[Apache UDP] Error al crear socket UDP. Error: " << WSAGetLastError() << std::endl;
        return;
    }

    // Bindeamos el socket al puerto de recepción (recibir posiciones de otros jugadores)
    sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(udpRecvPort);   // Puerto de recepción

    if (bind(udpSocket, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
        std::cout << "[Apache UDP] Error al bindear socket de recepción (Cliente). Error: " << WSAGetLastError() << std::endl;
        closesocket(udpSocket);
        return;
    }

    // Configura la dirección del servidor UDP (donde enviaremos nuestras posiciones)
    udpServer.sin_family = AF_INET;
    udpServer.sin_port = htons(udpSendPort);   // Puerto donde escucha el servidor UDP (7778)
    int result = inet_pton(AF_INET, ip.c_str(), &udpServer.sin_addr);
    if (result <= 0) {
        std::cout << "[Apache UDP] inet_pton FALLÓ! Resultado: " << result << std::endl;
        closesocket(udpSocket);
        return;
    }

    std::cout << "[Apache UDP] Conectado al servidor UDP en " << ip << ":" << udpSendPort << std::endl;
    std::cout << "[Apache UDP] Escuchando en puerto local " << udpRecvPort << std::endl;

    // Iniciamos el thread de recepción (opcional si no lo tienes en otro lado)
    std::thread udpRecvThread(ReceiveRemotePositions, udpRecvPort);
    udpRecvThread.detach();

    clientRunning = true;

    // Bucle para enviar la posición de este jugador al servidor
    while (clientRunning)
    {
        SendMyPosition(actorId);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));  // 50 updates por segundo aprox
    }

    closesocket(udpSocket);
}


void StartApacheClient(const ApacheConfig& config)
{
    IniciaWinsockUnaVez();

    // Activamos el loop
    clientRunning = true;

    std::cout << "[Apache TCP] IP de conexión: " << config.serverIP << std::endl;

    // TCP cliente
    std::thread tcpThread(ConnectTCP, config.serverIP, config.tcpPort);
    tcpThread.detach();

    // UDP envío
    std::thread udpSendThread(StartUDP, config.serverIP, config.udpSendPort, config.udpRecvPort, config.actorId);
    udpSendThread.detach();

    // UDP recepción
    std::thread udpRecvThread(ReceiveRemotePositions, config.udpRecvPort);
    udpRecvThread.detach();

    std::cout << "[Apache] Threads de Cliente iniciados." << std::endl;
    SafeLog("[CLIENT] Iniciando cliente");
}

void StopApacheClient()
{
    // Cortamos el loop de los threads
    clientRunning = false;

    // Cerramos el socket UDP si está abierto
    if (udpSocket != INVALID_SOCKET)
    {
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET;  // Para evitar cierres dobles
        std::cout << "[Apache] UDP Socket cerrado!" << std::endl;
    }

    // Cerramos el socket TCP si está abierto
    if (tcpSocket != INVALID_SOCKET)
    {
        closesocket(tcpSocket);
        tcpSocket = INVALID_SOCKET;
        std::cout << "[Apache] TCP Socket cerrado!" << std::endl;
    }

    std::cout << "[Apache] Cliente detenido correctamente!" << std::endl;
    SafeLog("[CLIENT] Cliente detenido correctamente");
}









