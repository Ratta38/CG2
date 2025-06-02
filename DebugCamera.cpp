#include "DebugCamera.h"
#include "MathUtility.h"
#include <numbers>
void DebugCamera::Initialize() {
	orientation_ = MathUtility::MakeIdentity4x4(); 
	translation_ = {0.0f, 0.0f, -50.0f};           
	pivot_ = {0.0f, 0.0f, 0.0f};                  

	UpdateViewMatrix();
}

void DebugCamera::SetPivot(const Vector3& p) {
	Vector3 offset = MathUtility::Subtract(translation_, pivot_);
	pivot_ = p;
	translation_ = MathUtility::Add(pivot_, offset);
}

void DebugCamera::Update(const DirectInput& input) {
	// フリー回転
	if (input.IsMouseButtonDown(1)) {
		float dx = input.GetMouseDeltaX() * 0.001f; 
		float dy = input.GetMouseDeltaY() * 0.001f;

		Matrix4x4 yaw = MathUtility::MakeYawRotateMatrix(dx);
		Matrix4x4 pitch = MathUtility::MakePitchRotateMatrix(dy);

		orientation_ = MathUtility::Multiply(pitch, MathUtility::Multiply(yaw, orientation_));
		orientation_ = MathUtility::Orthonormalize(orientation_);
	}

	// ピボット回転
	if (input.IsMouseButtonDown(2)) {
		float dx = input.GetMouseDeltaX() * 0.001f;
		float dy = input.GetMouseDeltaY() * 0.001f;

		Matrix4x4 yaw = MathUtility::MakeYawRotateMatrix(dx);
		Matrix4x4 pitch = MathUtility::MakePitchRotateMatrix(dy);
		Matrix4x4 rot = MathUtility::Multiply(pitch, yaw);

		Vector3 offset = MathUtility::Subtract(translation_, pivot_);
		offset = MathUtility::MultiplyVector(offset, rot);
		translation_ = MathUtility::Add(pivot_, offset);

		orientation_ = MathUtility::Multiply(rot, orientation_);
		orientation_ = MathUtility::Orthonormalize(orientation_);
	}

	auto MoveLocal = [&](const Vector3& local) {
		Vector3 world = MathUtility::MultiplyVector(local, orientation_);
		translation_ = MathUtility::Add(translation_, world);
		pivot_ = MathUtility::Add(pivot_, world);
	};

	const float speed = 0.1f;

	if (!input.IsKeyDown(DIK_LSHIFT)) {
		// 前移動
		if (input.IsKeyDown(DIK_W)) {
			MoveLocal({0.0f, 0.0f, speed});
		}

		// 後ろ移動
		if (input.IsKeyDown(DIK_S)) {
			MoveLocal({0.0f, 0.0f, -speed});
		}
	}

	if (input.IsKeyDown(DIK_LSHIFT)) {
		// 右移動
		if (input.IsKeyDown(DIK_D)) {
			MoveLocal({speed, 0.0f, 0.0f});
		}

		// 左移動
		if (input.IsKeyDown(DIK_A)) {
			MoveLocal({-speed, 0.0f, 0.0f});
		}

		// 上移動
		if (input.IsKeyDown(DIK_W)) {
			MoveLocal({0.0f, speed, 0.0f});
		}

		// 下移動
		if (input.IsKeyDown(DIK_S)) {
			MoveLocal({0.0f, -speed, 0.0f});
		}
	}

	UpdateViewMatrix();
}

void DebugCamera::UpdateViewMatrix() {
	Matrix4x4 world = MathUtility::Multiply(orientation_, MathUtility::MakeTranslateMatrix(translation_));
	viewMatrix_ = MathUtility::Inverse(world);
}
