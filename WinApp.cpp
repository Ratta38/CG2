#include "WinApp.h"

WinApp::WinApp() {
	// ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc_.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録する
	RegisterClass(&wc_);

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = {0, 0, kClientWidth, kClientHeight};

	// クライアント領域を基に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	hwnd_ = CreateWindow(
	    wc_.lpszClassName,     // 利用するクラス名
	    L"CG2",               // タイトルバーの文字
	    WS_OVERLAPPEDWINDOW,  // よく見るウィンドウスタイル
	    CW_USEDEFAULT,        // 表示X座標 (Windowsに任せる)
	    CW_USEDEFAULT,        // 表示Y座標 (WindowsOSに任せる)
	    wrc.right - wrc.left, // ウィンドウ横幅
	    wrc.bottom - wrc.top, // ウィンドウ縦幅
	    nullptr,              // 親ウィンドウハンドル
	    nullptr,              // メニューハンドル
	    wc_.hInstance,         // インスタンスハンドル
	    nullptr);             // オプション

	// ウィンドウを表示する
	ShowWindow(hwnd_, SW_SHOW);
}

WinApp::~WinApp() { 
	UnregisterClass(wc_.lpszClassName, wc_.hInstance);
}

bool WinApp::ProcessMessage() { 
	MSG msg{};
	// Windowにメッセージが来てたら最優先で処理させる
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対してゲーム固有の処理を行う
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
