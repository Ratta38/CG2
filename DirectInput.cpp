#include "DirectInput.h"
#include <assert.h>

DirectInput::~DirectInput() {}

void DirectInput::Initialize(HINSTANCE hInstance, HWND hwnd) {
	HRESULT result;

	// DirectInputの初期化
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, keyboard_.GetAddressOf(), NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void DirectInput::Update() {
	keyboard_->Acquire();

	// 現フレーム → 前フレームへコピー
	memcpy(preKey_, key_, sizeof(key_));

	// 最新の状態を取得
	keyboard_->GetDeviceState(sizeof(key_), key_);
}

bool DirectInput::IsKeyDown(uint8_t keyCode) const {
	// 0でなければ押している
	if (key_[keyCode]) {
		return true;
	}
	// 押していない
	return false;
}

bool DirectInput::IsKeyTriggered(uint8_t keyCode) const {
	// 前回が0で、今回が0でなければトリガー
	if (!preKey_[keyCode] && key_[keyCode]) {
		return true;
	}
	// トリガーでない
	return false;
}

bool DirectInput::IsKeyReleased(uint8_t keyCode) const {
	// 前回が0でなく、今回が0であればトリガー
	if (preKey_[keyCode] && !key_[keyCode]) {
		return true;
	}
	// トリガーでない
	return false;
}
