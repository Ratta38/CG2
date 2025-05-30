#include "DebugCamera.h"
#include "MathUtility.h"
#include <numbers>
#include <algorithm>

void DebugCamera::Initialize(Matrix4x4 cameraMatrix) { 
	rotation_ = {0, 0, 0};
	translation_ = {0, 0, -50};
	viewMatrix_ = MathUtility::Inverse(cameraMatrix);
	projectionMatrix_ = MathUtility::MakePerspectiveFovMatrix(0.45f, (1280.0f / 720.0f), 0.1f, 100.0f);
}

void DebugCamera::Update(const DirectInput& input) {
	// 左クリックでカメラ回転開始
	if (input.IsMouseButtonDown(1)) {
		float dx = input.GetMouseDeltaX() * 0.001f;
		float dy = input.GetMouseDeltaY() * 0.001f;

		rotation_.x += dy;
		rotation_.y += dx;
	}

	// 前後移動
	if (input.IsKeyDown(DIK_W)) {
		const float speed = 0.1f;

		// カメラ移動ベクトル
		Vector3 move = {0, 0, speed};

		// 移動ベクトルを角度分だけ回転させる
		Matrix4x4 rotY = MathUtility::MakeYawRotateMatrix(rotation_.x);
		Matrix4x4 rotX = MathUtility::MakePitchRotateMatrix(rotation_.y);
		Matrix4x4 rotate = MathUtility::Multiply(rotY, rotX);
		Vector3 rotatedMove = MathUtility::MultiplyVector(move, rotate);

		// 移動ベクトル分だけ座標を加算する
		translation_ = MathUtility::Add(translation_, rotatedMove);
	}

	if (input.IsKeyDown(DIK_S)) {
		const float speed = -0.1f;

		// カメラ移動ベクトル
		Vector3 move = {0, 0, speed};

		// 移動ベクトルを角度分だけ回転させる
		Matrix4x4 rotY = MathUtility::MakeYawRotateMatrix(rotation_.x);
		Matrix4x4 rotX = MathUtility::MakePitchRotateMatrix(rotation_.y);
		Matrix4x4 rotate = MathUtility::Multiply(rotY, rotX);
		Vector3 rotatedMove = MathUtility::MultiplyVector(move, rotate);

		// 移動ベクトル分だけ座標を加算する
		translation_ = MathUtility::Add(translation_, rotatedMove);
	}

	// 左右移動
	if (input.IsKeyDown(DIK_D)) {
		const float speed = 0.1f;

		// カメラ移動ベクトル
		Vector3 move = {speed, 0, 0};

		// 移動ベクトルを角度分だけ回転させる
		Matrix4x4 rotY = MathUtility::MakeYawRotateMatrix(rotation_.x);
		Matrix4x4 rotX = MathUtility::MakePitchRotateMatrix(rotation_.y);
		Matrix4x4 rotate = MathUtility::Multiply(rotY, rotX);
		Vector3 rotatedMove = MathUtility::MultiplyVector(move, rotate);

		// 移動ベクトル分だけ座標を加算する
		translation_ = MathUtility::Add(translation_, rotatedMove);
	}

	if (input.IsKeyDown(DIK_A)) {
		const float speed = -0.1f;

		// カメラ移動ベクトル
		Vector3 move = {speed, 0, 0};

		// 移動ベクトルを角度分だけ回転させる
		Matrix4x4 rotY = MathUtility::MakeYawRotateMatrix(rotation_.x);
		Matrix4x4 rotX = MathUtility::MakePitchRotateMatrix(rotation_.y);
		Matrix4x4 rotate = MathUtility::Multiply(rotY, rotX);
		Vector3 rotatedMove = MathUtility::MultiplyVector(move, rotate);

		// 移動ベクトル分だけ座標を加算する
		translation_ = MathUtility::Add(translation_, rotatedMove);
	}

	Matrix4x4 rotateMatrix = MathUtility::Multiply(MathUtility::MakePitchRotateMatrix(rotation_.x), MathUtility::MakeYawRotateMatrix(rotation_.y));
	Matrix4x4 translateMatrix = MathUtility::MakeTranslateMatrix(translation_);
	Matrix4x4 worldMatrix = MathUtility::Multiply(rotateMatrix, translateMatrix);
	viewMatrix_ = MathUtility::Inverse(worldMatrix);
}