#include "script.h"
#include "keyboard.h"

void TeleportToArmadillo()
{
	Vector3 coords = { -2171.0f, 23.0f, 2592.0f };

	Actor localActor = PLAYER::GET_PLAYER_ACTOR(PLAYER::GET_LOCAL_SLOT());
	float heading = PLAYER::GET_HEADING(localActor);

	if (ACTOR::IS_ACTOR_VALID(localActor)) {
		PLAYER::TELEPORT_ACTOR_WITH_HEADING(localActor, coords.x, coords.y, coords.z, heading, false, false, false);
	}
}

void ScriptMain()
{
	srand(static_cast<unsigned int>(GetTickCount64()));
	while (true)
	{
		drawRect(0.5, 0.5, 0.2, 0.2, 0, 0, 0, 160, 0.0f);

		if (IsKeyJustUp(VK_F8))
			TeleportToArmadillo();

		scriptWait(0);
	}
}