#pragma once
#include <string>

struct ApacheConfig
{
    std::string serverIP;
    int tcpPort;
    int udpSendPort; // Para enviar datos
    int udpRecvPort; // Para escuchar los datos del otro
    int actorId;
    bool isServer; // Nuevo valor
};

// Cargar la config desde el archivo ini
ApacheConfig LoadApacheConfig(const std::string& filename);
