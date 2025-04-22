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
    if (!ENTITY::IS_ACTOR_VALID(dummy))
        return;

    // Anular tareas automaticas
    TASKS::TASK_CLEAR(dummy);

    // Congelar al Dummy fisicamente
    ENTITY::SET_MOVER_FROZEN(dummy, true);

    // Congelar animaciónes
    PHYSICS::SET_PHYSINST_FROZEN(ACTOR::GET_PHYSINST_FROM_ACTOR(dummy), true);

    // Protección opcional
    ACTOR::SET_ACTOR_INVULNERABILITY(dummy, true);
    ACTOR::SET_ACTOR_UNKILLABLE(dummy, true);

    printMessage("[Apache LAN] Dummy congelado exitosamente!");
}

Actor FindOrSpawnDummy(int actorId, Vector3 coords)
{
    printMessage("[Apache LAN] Entrando a FindOrSpawnDummy para actorId: " + std::to_string(actorId));

    // Verifica si el actor ya existe
    if (remoteActors.find(actorId) != remoteActors.end())
    {
        Actor existingActor = remoteActors[actorId];
        if (ENTITY::IS_ACTOR_VALID(existingActor)) {
            printMessage("[Apache LAN] Dummy ya existe y es válido! actorId: " + std::to_string(actorId));
            return existingActor;
        }
    }

    // Agregar validación antes de continuar
    Actor localActor = ACTOR::GET_PLAYER_ACTOR(ACTOR::GET_LOCAL_SLOT());
    if (!ENTITY::IS_ACTOR_VALID(localActor)) {
        printMessage("[Apache LAN] ERROR: No se pudo obtener el actor local!");
        return 0;
    }

    printMessage("[Apache LAN] Spawneando nuevo dummy remoto SIN REQUEST...");

    Vector3 spawnCoords = { coords.x, coords.y, coords.z };

    int actorEnum = 631;  // Jenny

    Actor newActor = OBJECT::CREATE_ACTOR_IN_LAYOUT(
        OBJECT::GET_AMBIENT_LAYOUT(),   // Puedes probar con FIND_NAMED_LAYOUT si es necesario
        "",
        actorEnum,
        Vector2(spawnCoords.x, spawnCoords.y),
        spawnCoords.z,
        Vector2(0.0f, 0.0f),
        0.0f
    );

    if (!ENTITY::IS_ACTOR_VALID(newActor))
    {
        printMessage("[Apache LAN] Dummy remoto NO creado!");
        return 0;
    }

    scriptWait(0); // Espera 1 frame para asegurar que se termine de spawnear

    ACTOR_DRAW::SET_DRAW_ACTOR(newActor, true);
    HUD::ADD_BLIP_FOR_ACTOR(newActor, 299, 0.0f, 1, 1);

    // Congelamos al dummy para que no se mueva ni actúe solo
    FreezeDummy(newActor);

    printMessage("[Apache LAN] Dummy creado y congelado! actorId: " + std::to_string(actorId));

    // Guardamos en el mapa
    remoteActors[actorId] = newActor;

    return newActor;
}










