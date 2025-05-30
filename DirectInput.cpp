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

	// マウス
	result = directInput_->CreateDevice(GUID_SysMouse, mouse_.GetAddressOf(), nullptr);
	assert(SUCCEEDED(result));
	// ホイール・追加ボタンを扱うので DIMOUSESTATE2
	result = mouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));
	// フルスクリーン時は EXCLUSIVE 推奨
	result = mouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}

void DirectInput::Update() {
	// キーボード
	keyboard_->Acquire();
	// 現フレーム → 前フレームへコピー
	memcpy(preKey_, key_, sizeof(key_));
	// 最新の状態を取得
	keyboard_->GetDeviceState(sizeof(key_), key_);

	// マウス
	mouse_->Acquire();
	preMouseState_ = mouseState_;
	std::memset(&mouseState_, 0, sizeof(mouseState_));
	mouse_->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState_);

	// 累積位置を作りたければここで加算
	accumX_ += mouseState_.lX;
	accumY_ += mouseState_.lY;
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

bool DirectInput::IsMouseButtonDown(uint8_t button) const { 
	return mouseState_.rgbButtons[button] & 0x80;
}
bool DirectInput::IsMouseButtonTriggered(uint8_t button) const { 
	return !(preMouseState_.rgbButtons[button] & 0x80) && (mouseState_.rgbButtons[button] & 0x80);
}
bool DirectInput::IsMouseButtonReleased(uint8_t button) const { 
	return (preMouseState_.rgbButtons[button] & 0x80) && !(mouseState_.rgbButtons[button] & 0x80);
}
