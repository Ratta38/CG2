#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "WinApp.h"
#include "D3DResourceLeakChecker.h"
#include <memory>
#include "SphereMeshGenerator.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	CoInitializeEx(0, COINIT_MULTITHREADED);

	// リリースリークチェック
	D3DResourceLeakChecker leakCheck;

	// ウィンドウ作成
	auto winApp = std::make_shared<WinApp>();

	// DirectX12 デバイス初期化
	auto dxCommon = std::make_unique<DirectXCommon>();
	dxCommon->Initialize(winApp);

	// テクスチャマネージャー
	std::unique_ptr<TextureManager> textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxCommon->GetDevice(), dxCommon->GetSrvDescriptorHeap().Get());

	// ImGuiManager
	std::unique_ptr<ImGuiManager> imGuiManager_ = std::make_unique<ImGuiManager>();
	imGuiManager_->Initialize(
	    dxCommon->GetWinApp()->GetHWND(), dxCommon->GetDevice(), dxCommon->GetSwapChainDescBufferCount(), dxCommon->GetRtvFormat(), dxCommon->GetSrvDescriptorHeap().Get());

	SphereMeshGenerator sphereMesh(16);

	// Model
	auto model = std::make_shared<Model>();
	model->Initialize(dxCommon->GetDevice(), sphereMesh);

	// Sprite
	auto sprite = std::make_shared<Sprite>();
	sprite->Initialize(dxCommon->GetDevice());

	// ウィンドウのxボタンが押されるまでループ
	while (dxCommon->GetWinApp()->ProcessMessage()) {

		imGuiManager_->BeginFrame();

		// ゲームの処理
		// 三角形
		model->Update();

		// スプライト
		sprite->Update();

		imGuiManager_->UpdateUI(model->GetColor(), sprite->GetTransform());

		// 描画開始
		dxCommon->BeginFrame();

		// 3Dモデル描画
		model->SetPipelineState(dxCommon->GetGraphicsPipelineState());
		model->SetRootSignature(dxCommon->GetRootSignature());
		model->SetTextureHandle(textureManager_->GetTextureSrvHandleGPU());

		// 球描画
		model->Draw(dxCommon->GetCommandList());

		// スプライト描画
		sprite->Draw(dxCommon->GetCommandList());

		// ImGuiの内部コマンドを生成する
		imGuiManager_->Render(dxCommon->GetCommandList());

		// 描画終了
		dxCommon->EndFrame();
	}

	imGuiManager_->Finalize();

	CloseHandle(dxCommon->GetFenceEvent());

	CoUninitialize();
	return 0;
}