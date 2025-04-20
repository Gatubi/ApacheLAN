#pragma once
#include <string>
#include <iostream>
#include "ApacheServer.h"
#include "ApacheClient.h"
#include "types.h"
#include "natives.h"
#include <unordered_map>
#include "script.h" // Ruta correcta según la estructura de ScriptHookRDR

//struct Vector2 {
//    float x;
//    float y;
//    Vector2(float _x, float _y) : x(_x), y(_y) {}
//};

void printMessage(std::string message);

extern std::unordered_map<int, Actor> remoteActors;

#pragma pack(push, 1)
struct PositionPacket
{
    int actorId;     // ID único del jugador remoto
    double posX;
    double posY;
    double posZ;
};
#pragma pack(pop)

int SpawnRemotePlayer(Vector3 pos, const char* model);

Actor FindOrSpawnDummy(int actorId, Vector3 coords);

void IniciaWinsockUnaVez();

void SafeLog(const std::string& message);

extern bool serverRunning;
extern bool clientRunning;


//Actor SpawnRemotePlayer(Vector3 coords, const char* model)
//{
//    int actorEnum = OBJECT::GET_ACTOR_ENUM_FROM_STRING(model);
//    Actor new_actor = OBJECT::CREATE_ACTOR_IN_LAYOUT(OBJECT::GET_AMBIENT_LAYOUT(), "", actorEnum, Vector2(coords.x, coords.z), coords.y, Vector2(0.00f, 0.00f), 0.00f);
//
//    if (ENTITY::IS_ACTOR_VALID(new_actor))
//    {
//        ACTOR::TELEPORT_ACTOR_WITH_HEADING(new_actor, Vector2(coords.x, coords.y), coords.z, 0.0f, false, false, false);
//        printMessage("[Apache LAN] Dummy creado correctamente");
//        std::cout << "[Apache LAN] Dummy creado en: " << coords.x << ", " << coords.y << ", " << coords.z << std::endl;
//    }
//    return new_actor;
//}