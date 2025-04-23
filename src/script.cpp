#include "ApacheServer.h"
#include "ApacheClient.h"
#include "ApacheUtils.h"
#include "ApacheConfig.h"
#include "types.h"
#include "script.h"
#include "keyboard.h"
#include "natives.h"
#include <string>
#include <fstream>
#include <format>
#include <vector>
#include <thread>

void InitializeConsole()
{
	AllocConsole(); // Crea la consola
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);  // Redirige std::cout
	freopen_s(&file, "CONOUT$", "w", stderr);  // Redirige std::cerr
	std::cout.clear(); // Limpia los streams
	std::cerr.clear();

	// Mensaje de confirmación
	std::cout << "Consola ApacheLAN activa!" << std::endl;
}

void SafeLog(const std::string& message)
{
	std::ofstream logFile("Apache_Debug.log", std::ios::app);
	if (logFile.is_open())
	{
		logFile << message << std::endl;
		logFile.close();
	}
}

void PrecacheActors()
{
	printMessage("[Apache LAN] Precarga omitida intencionalmente!");
}

//Actor SpawnRemotePlayer() {
//
//	Actor localActor = ACTOR::GET_PLAYER_ACTOR(0);
//
//	Vector3 playerCoords;
//	ACTOR::GET_POSITION(localActor, &playerCoords);
//
//	Vector3 spawnCoords = { playerCoords.x + 2.0f , playerCoords.y, playerCoords.z + 2.0f };
//
//	int actorEnum = 631; // Jenny
//	STREAM::STREAMING_REQUEST_ACTOR(actorEnum, true, false);
//	while (!STREAM::STREAMING_IS_ACTOR_LOADED(actorEnum, -1)) {
//		WAIT(0);
//	}
//
//	Actor newActor = OBJECT::CREATE_ACTOR_IN_LAYOUT(OBJECT::FIND_NAMED_LAYOUT("PlayerLayout"), "", actorEnum, spawnCoords, Vector3(0.0f, 0.0f, 0.0f));
//
//	ACTOR_DRAW::SET_DRAW_ACTOR(newActor, true);
//
//	if (STREAM::STREAMING_IS_ACTOR_LOADED(actorEnum, -1))
//		STREAM::STREAMING_EVICT_ACTOR(actorEnum, -1);
//
//	HUD::ADD_BLIP_FOR_ACTOR(newActor, 299, 0.0f, 1, 1);
//
//	return newActor;
//}


//void TeleportToArmadillo()
//{
//	Vector3 coords = { -2171.0f, 23.0f, 2592.0f };
//
//	Actor localActor = ACTOR::GET_PLAYER_ACTOR(ACTOR::GET_LOCAL_SLOT());
//	float heading = ACTOR::GET_HEADING(localActor);
//	if (ENTITY::IS_ACTOR_VALID(localActor)) {
//		ACTOR::TELEPORT_ACTOR_WITH_HEADING(localActor, Vector2(coords.x, coords.y), coords.z, heading, false, false, false);
//	}
//}

void PrintStatic()
{
	int static_ = (int)*getStaticPtr("$/content/scripting/designerdefined/short_update_thread", 119);
	std::string msg = std::format("Static_119: {}", (float)static_);
	HUD::HUD_CLEAR_OBJECTIVE_QUEUE();
	HUD::PRINT_OBJECTIVE_B(msg.c_str(), 2.0f, true, 2, 1, 0, 0, 0);
}

void printGlobal()
{
	int ptr = (int)*getGlobalPtr(54086);
	std::string msg = std::format("Stat: {}", ptr);
	HUD::HUD_CLEAR_OBJECTIVE_QUEUE();
	HUD::PRINT_OBJECTIVE_B(msg.c_str(), 2.0f, true, 2, 1, 0, 0, 0);
}

void printMessage(std::string msg) {
	HUD::HUD_CLEAR_OBJECTIVE_QUEUE();
	HUD::PRINT_OBJECTIVE_B(msg.c_str(), 2.0f, true, 2, 1, 0, 0, 0);
}

void KillAllActors()
{
	constexpr int SIZE = 100;
	int actors[SIZE];

	int count = worldGetAllActors(actors, SIZE);

	for (int i = 0; i < count; i++) {
		if (!ENTITY::IS_ACTOR_VALID(actors[i])) continue;
		if (ACTOR::IS_ACTOR_LOCAL_PLAYER(actors[i])) continue;

		HEALTH::KILL_ACTOR(actors[i]);
	}
}

void GiveMonyFromScript()
{
	std::vector<u64> args{ 1000, 1, 1 };
	scriptCall("$/content/main", 107871, (u32)args.size(), args.data());
}

void ScriptMain()
{
    InitializeConsole();
    printMessage("Apache LAN está despierto...");

    srand(static_cast<unsigned int>(GetTickCount64()));

    // Cargamos el archivo ApacheConfig.ini
    ApacheConfig config = LoadApacheConfig("ApacheConfig.ini");

	static bool apacheActivo = false;
	static bool partidaCargada = false;
	static bool serverIniciado = false;
	static bool clienteIniciado = false;
	static bool modelosPrecargados = false;


	while (true)
	{
		Actor localActor = ACTOR::GET_PLAYER_ACTOR(ACTOR::GET_LOCAL_SLOT());

		if (!partidaCargada && ENTITY::IS_ACTOR_VALID(localActor))
		{
			partidaCargada = true;
			printMessage("[Apache LAN] Partida detectada! Esperando inicio manual...");
		}

		if (partidaCargada && !modelosPrecargados && IsKeyJustUp(VK_F9)) // F9 para iniciar cliente/servidor manualmente
		{
			PrecacheActors();
			modelosPrecargados = true;
			if (config.isServer && !serverIniciado)
			{
				std::thread serverThread(StartApacheServer);
				serverThread.detach();

				printMessage("[Apache LAN] Servidor iniciado!");
				serverIniciado = true;
			}

			// Solo los clientes o el propio servidor si participa
			if (!clienteIniciado)
			{
				std::thread clientThread(StartApacheClient, config);
				clientThread.detach();
				clienteIniciado = true;
			}
		}

		if (partidaCargada && IsKeyJustUp(VK_F10))
		{
			if (!clienteIniciado)
			{
				std::thread clientThread(StartApacheClient, config);
				clientThread.detach();

				printMessage("[Apache LAN] Cliente iniciado!");
				clienteIniciado = true;
			}
			else
			{
				StopApacheClient();

				printMessage("[Apache LAN] Cliente detenido!");
				clienteIniciado = false;
			}
		}


		// Toggle del Servidor con F11
		if (partidaCargada && IsKeyJustUp(VK_F11))
		{
			if (!serverIniciado)
			{
				std::thread serverThread(StartApacheServer);
				serverThread.detach();

				printMessage("[Apache LAN] Servidor iniciado!");
				serverIniciado = true;
			}
			else
			{
				StopApacheServer();

				printMessage("[Apache LAN] Servidor detenido!");
				serverIniciado = false;
			}
		}

		drawText(0.5f, 0.5f,
			"<outline><00FF00>Hola Mundo Apache!</00FF00></outline> "
			"<shadow><33c4ff>Apache Modding RDR1</33c4ff></shadow> "
			"<0xFcAf17>#RespetaAlModder</0xFcAf17>",
			255, 255, 255, 255, s_CustomFontId2, 0.05f, Center);

		scriptWait(0);
	}

}
