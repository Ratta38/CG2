#pragma once
#include <Windows.h>
struct MouseState
{
	float x;
	float y;
	float z;
	BYTE rgbButtons[8];
};