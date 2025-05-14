#pragma once
#include "Vector4.h"
#include <widemath.h>
struct Material {
	Vector4 color;
	int32_t enableLighting;
};