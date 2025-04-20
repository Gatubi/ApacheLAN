#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "ApacheConfig.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Función auxiliar para parsear líneas "clave=valor"
std::string GetValue(const std::string& line)
{
    size_t pos = line.find('=');
    if (pos == std::string::npos) return "";

    std::string value = line.substr(pos + 1);

    // Trim de espacios y saltos de línea
    value.erase(0, value.find_first_not_of(" \t\n\r"));
    value.erase(value.find_last_not_of(" \t\n\r") + 1);

    return value;
}


ApacheConfig LoadApacheConfig(const std::string& filename)
{
    ApacheConfig config;

    // Valores por defecto
    config.serverIP = "127.0.0.1";
    config.tcpPort = 7777;
    config.udpSendPort = 7778;
    config.udpRecvPort = 7779;
    config.actorId = 1;
    config.isServer = false;  // Por defecto NO es servidor

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "[ApacheConfig] No se pudo abrir el archivo de configuración, usando valores por defecto." << std::endl;
        return config;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Ignora líneas vacías o comentarios
        if (line.empty() || line[0] == '#')
            continue;

        if (line.find("serverIP") != std::string::npos)
            config.serverIP = GetValue(line);

        else if (line.find("tcpPort") != std::string::npos)
            config.tcpPort = std::stoi(GetValue(line));

        else if (line.find("udpSendPort") != std::string::npos)
            config.udpSendPort = std::stoi(GetValue(line));

        else if (line.find("udpRecvPort") != std::string::npos)
            config.udpRecvPort = std::stoi(GetValue(line));

        else if (line.find("actorId") != std::string::npos)
            config.actorId = std::stoi(GetValue(line));

        else if (line.find("isServer") != std::string::npos)
            config.isServer = (GetValue(line) == "true");
    }

    file.close();

    std::cout << "[ApacheConfig] Configuración cargada correctamente." << std::endl;
    std::cout << "[ApacheConfig] serverIP: " << config.serverIP << std::endl;
    std::cout << "[ApacheConfig] tcpPort: " << config.tcpPort << std::endl;
    std::cout << "[ApacheConfig] udpSendPort: " << config.udpSendPort << std::endl;
    std::cout << "[ApacheConfig] udpRecvPort: " << config.udpRecvPort << std::endl;
    std::cout << "[ApacheConfig] actorId: " << config.actorId << std::endl;
    std::cout << "[ApacheConfig] isServer: " << (config.isServer ? "true" : "false") << std::endl;

    std::cout << "[ApacheConfig] serverIP: [" << config.serverIP << "]" << std::endl;

    return config;
}
