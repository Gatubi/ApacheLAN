#include "natives.h"
#include "ApacheUtils.h"

// Flag para controlar si ya inicializamos Winsock
bool winsockInicializado = false;

bool serverRunning = false;
bool clientRunning = false;

void IniciaWinsockUnaVez()
{
    if (!winsockInicializado)
    {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

        if (result != 0)
        {
            std::cout << "[Apache] Error inicializando Winsock: " << result << std::endl;
            return;
        }

        winsockInicializado = true;
        std::cout << "[Apache] Winsock inicializado correctamente!" << std::endl;
    }
}

void FreezeDummy(Actor dummy)
{
    if (!ENTITY::IS_ACTOR_VALID(dummy)) {
        printMessage("[Apache LAN] El dummy recibido NO es válido. FreezeDummy cancelada.");
        return;
    }


    printMessage("[Apache LAN] Dummy congelado exitosamente!");
}

Actor FindOrSpawnDummy(int actorId, Vector3 coords)
{
    printMessage("[Apache LAN] Entrando a FindOrSpawnDummy para actorId: " + std::to_string(actorId));

    // Verifica si ya está creado
    if (remoteActors.find(actorId) != remoteActors.end()) {
        Actor existingActor = remoteActors[actorId];
        if (ENTITY::IS_ACTOR_VALID(existingActor)) {
            printMessage("[Apache LAN] Dummy ya existe y es válido! actorId: " + std::to_string(actorId));
            return existingActor;
        }
    }

    // Setup de coordenadas de ubicacion
    Vector3 spawnCoords = { coords.x, coords.y, coords.z };
    int actorEnum = 631;  // Jenny u otro actor

    printMessage("[Apache LAN] Actor cargado, procediendo a spawn...");

    Actor newActor = OBJECT::CREATE_ACTOR_IN_LAYOUT(
        OBJECT::FIND_NAMED_LAYOUT("PlayerLayout"),
        "",
        actorEnum,
        Vector2(spawnCoords.x, spawnCoords.y),
        spawnCoords.z,
        Vector2(0.0f, 0.0f),
        0.0f
    );

    if (!ENTITY::IS_ACTOR_VALID(newActor)) {
        printMessage("[Apache LAN] Dummy remoto NO creado!");
        return 0;
    }

    // Congelar el actor
    TASKS::TASK_CLEAR(newActor);
    TASKS::TASK_STAND_STILL(newActor, -1.f, 0, 0); // Requiere estar después del spawn y esperar un poco
    ACTOR_DRAW::SET_DRAW_ACTOR(newActor, true);
    HUD::ADD_BLIP_FOR_ACTOR(newActor, 299, 0.0f, 1, 1);

    printMessage("[Apache LAN] Dummy creado y congelado! actorId: " + std::to_string(actorId));
    remoteActors[actorId] = newActor;

    return newActor;
}





