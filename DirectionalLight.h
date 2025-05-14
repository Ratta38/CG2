#pragma once
#include "Vector3.h"
#include "Vector4.h"
struct DirectionalLight
{
	Vector4 color; //!< ライトの色
	Vector3 direction; //!< ライトの向き
	float intensity; //!< 輝度
};