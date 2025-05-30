#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "DirectInput.h"

class DebugCamera {
public:
	void Initialize(Matrix4x4 cameraMatrix);
	void Update(const DirectInput& input);
	Matrix4x4 GetViewMatrix() { return viewMatrix_; }

private:
	// X,Y,X軸回りのローカル回転角
	Vector3 rotation_;
	// ローカル座標
	Vector3 translation_;
	// ビュー行列
	Matrix4x4 viewMatrix_;
	// 射影行列
	Matrix4x4 projectionMatrix_;
};
