#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "DirectInput.h"

class DebugCamera {
public:
	void Initialize();
	void SetPivot(const Vector3& p);
	void Update(const DirectInput& input);
	void UpdateViewMatrix();
	Matrix4x4 GetViewMatrix() { return viewMatrix_; }

private:
	// 累積回転行列
	Matrix4x4 orientation_;
	// ローカル座標
	Vector3 translation_;
	// ビュー行列
	Matrix4x4 viewMatrix_;
	// 射影行列
	Matrix4x4 projectionMatrix_;
	// ピボット
	Vector3 pivot_;
};
