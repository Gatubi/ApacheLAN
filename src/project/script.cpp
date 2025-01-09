#include "script.h"
#include "keyboard.h"
#include <string>
#include <format>

void TeleportToArmadillo()
{
	Vector3 coords = { -2171.0f, 23.0f, 2592.0f };

	Actor localActor = ACTOR::GET_PLAYER_ACTOR(ACTOR::GET_LOCAL_SLOT());
	float heading = ACTOR::GET_HEADING(localActor);
	if (ENTITY::IS_ACTOR_VALID(localActor)) {
		ACTOR::TELEPORT_ACTOR_WITH_HEADING(localActor, Vector2(coords.x, coords.y), coords.z, heading, false, false, false);
	}
}

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


void ScriptMain()
{
	srand(static_cast<unsigned int>(GetTickCount64()));
	while (true)
	{
		//drawRect(0.5f, 0.5f, 0.2f, 0.2f, 0, 0, 0, 160, 0.0f);
		//drawText(0.5f, 0.5f, std::format("Font Id: {}", s_CustomFontId).c_str(), 255, 255, 255, 255, s_CustomFontId, 0.05f, Left);
		//drawText(0.8f, 0.8f, std::format("Font Id: {}", s_CustomFontId2).c_str(), 255, 255, 255, 255, s_CustomFontId2, 0.05f, Left);

		if (IsKeyJustUp(VK_F8))
			TeleportToArmadillo();

		if (IsKeyJustUp(VK_F11))
			KillAllActors();

		scriptWait(0);
	}
}