#pragma once
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>
#include <memory>
#include <wrl.h>
#include <functional>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class DirectInput {
public:
	~DirectInput();
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	void Update();

	bool IsKeyDown(uint8_t keyCode) const;
	bool IsKeyTriggered(uint8_t keyCode) const;
	bool IsKeyReleased(uint8_t keyCode) const;

private:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;

	uint8_t key_[256];
	uint8_t preKey_[256];
	HINSTANCE hInstance_;
};
