#pragma once
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include "MouseState.h"
#include <dinput.h>
#include <functional>
#include <memory>
#include <wrl.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class DirectInput {
public:
	~DirectInput();
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	void Update();

	// キーボード
	bool IsKeyDown(uint8_t keyCode) const;
	bool IsKeyTriggered(uint8_t keyCode) const;
	bool IsKeyReleased(uint8_t keyCode) const;

	// マウス
	bool IsMouseButtonDown(uint8_t button) const;
	bool IsMouseButtonTriggered(uint8_t button) const;
	bool IsMouseButtonReleased(uint8_t button) const;
	float GetMouseDeltaX() const { return static_cast<float>(mouseState_.lX); }
	float GetMouseDeltaY() const { return static_cast<float>(mouseState_.lY); }
	float GetMouseWheel() const { return static_cast<float>(mouseState_.lZ); }

private:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;

	// キーボード
	uint8_t key_[256];
	uint8_t preKey_[256];
	HINSTANCE hInstance_;

	// マウス
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_;
	DIMOUSESTATE2 mouseState_{};    // 今フレームのマウス入力
	DIMOUSESTATE2 preMouseState_{}; // 前フレーム

	float accumX_ = 0;
	float accumY_ = 0;
};
