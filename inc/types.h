#pragma once

#include <windows.h>

typedef DWORD Void;
typedef DWORD Any;
typedef DWORD uint;
typedef DWORD Hash;
typedef int Actor;
typedef int Player;
typedef int Vehicle;
typedef int Camera;
typedef int Cam;
typedef int Object;
typedef int Layout;
typedef int GUIWindow;
typedef int Blip;


#define ALIGN8 __declspec(align(8))

#pragma pack(push, 1)
typedef struct
{
	ALIGN8 float x;
	ALIGN8 float y;
	ALIGN8 float z;
} Vector3;
#pragma pack(pop)

typedef struct 
{ 
	float x, y;
} Vector2;

typedef struct
{ 
	int R, G, B, A; 
} RGBA;