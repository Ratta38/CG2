#pragma once
#include "Vector3.h"
struct FogParam {
	Vector3 fogCenter;  // フォグの中心座標
	float radius;       // 有効範囲
	Vector3 fogColor;   // フォグの色
	float fogIntensity; // 最大の霧の濃さ
};