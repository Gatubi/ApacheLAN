#include "script.h"
#include "keyboard.h"
#include <string>
#include <format>

void TeleportToArmadillo()
{
	Vector3 coords = { -2171.0f, 23.0f, 2592.0f };

	Actor localActor = PLAYER::GET_PLAYER_ACTOR(PLAYER::GET_LOCAL_SLOT());
	float heading = PLAYER::GET_HEADING(localActor);

	u64 combined = (static_cast<u64>(*reinterpret_cast<u32*>(&coords.y)) << 32) | *reinterpret_cast<u32*>(&coords.x);
	if (ACTOR::IS_ACTOR_VALID(localActor)) {
		PLAYER::TELEPORT_ACTOR_WITH_HEADING(localActor, combined, coords.z, heading, false, false, false);
	}
}


void PrintStatic()
{
	int static_ = (int)*getStaticPtr("$/content/scripting/designerdefined/short_update_thread", 119);
	std::string msg = std::format("Static_119: {}", (float)static_);
	HUD::HUD_CLEAR_OBJECTIVE_QUEUE();
	HUD::_PRINT_SUBTITLE(msg.c_str(), 2.0f, true, 2, 1, 0, 0, 0);
}


void ScriptMain()
{
	srand(static_cast<unsigned int>(GetTickCount64()));
	while (true)
	{
		drawRect(0.5f, 0.5f, 0.2f, 0.2f, 0, 0, 0, 160, 0.0f);
		drawText(0.5f, 0.5f, std::format("Font Id: {}", s_CustomFontId).c_str(), 255, 255, 255, 255, s_CustomFontId, 0.05f, Left);
		drawText(0.8f, 0.8f, std::format("Font Id: {}", s_CustomFontId2).c_str(), 255, 255, 255, 255, s_CustomFontId2, 0.05f, Left);

		if (IsKeyJustUp(VK_F8))
			TeleportToArmadillo();

		if (IsKeyJustUp(VK_F9))
			PrintStatic();

		scriptWait(0);
	}
}