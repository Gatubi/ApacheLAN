#include "..\..\inc\main.h"
#include "script.h"
#include "keyboard.h"

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		//MessageBox(0, "", "", 0);
		scriptRegister(hInstance, ScriptMain);

		s_CustomFontId = getCustomFontByPath("redo.ttf");
		if (s_CustomFontId == -1) {
			s_CustomFontId = registerFont("redo.ttf", 72.0f);
		}

		s_CustomFontId2 = getCustomFontByPath("droid.ttf");
		if (s_CustomFontId2 == -1) {
			s_CustomFontId2 = registerFont("droid.ttf", 72.0f);
		}
		keyboardHandlerRegister(OnKeyboardMessage);
		break;
	case DLL_PROCESS_DETACH:
		scriptUnregister(hInstance);
		keyboardHandlerUnregister(OnKeyboardMessage);
		break;
	}
	return TRUE;
}