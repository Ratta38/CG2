#include "D3DResourceLeakChecker.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Model.h"
#include "SphereMeshGenerator.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "WinApp.h"
#include "XAudio.h"
#include "DirectInput.h"
#include "DebugCamera.h"
#include "MathUtility.h"
#include <memory>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
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

	// ImGuiManager
	std::unique_ptr<ImGuiManager> imGuiManager_ = std::make_unique<ImGuiManager>();
	imGuiManager_->Initialize(dxCommon->GetWinApp()->GetHWND(), dxCommon->GetDevice(), dxCommon->GetSwapChainDescBufferCount(), dxCommon->GetRtvFormat(), dxCommon->GetSrvDescriptorHeap().Get());

	SphereMeshGenerator sphereMesh(16);

	// Model
	auto model = std::make_shared<Model>();
	model->Initialize(dxCommon->GetDevice(), sphereMesh);

	// Sprite
	auto sprite = std::make_shared<Sprite>();
	sprite->Initialize(dxCommon->GetDevice());

	// XAudio
	std::unique_ptr<XAudio> audio = std::make_unique<XAudio>();
	audio->Initialize();
	audio->SoundsAllLoad();

	// DirectInput
	std::unique_ptr<DirectInput> input = std::make_unique<DirectInput>();
	input->Initialize(hInstance, dxCommon->GetWinApp()->GetHWND());

	// DebugCamera
	std::unique_ptr<DebugCamera> debugCamera = std::make_unique<DebugCamera>();
	debugCamera->Initialize();

	textureManager_->Initialize(dxCommon->GetDevice(), dxCommon->GetSrvDescriptorHeap().Get(), model->GetModelData().material);
	sprite->SetSrvHandle(textureManager_->GetTextureSrvHandleGPU());

	// 音声再生
	//audio->SoundPlayWave(audio->GetXAudio2().Get(), audio->GetSound());

	//int count = 0;

	// ウィンドウのxボタンが押されるまでループ
	while (dxCommon->GetWinApp()->ProcessMessage()) {

		input->Update();

		imGuiManager_->BeginFrame();

		debugCamera->Update(*input);
		model->SetViewMatrix(debugCamera->GetViewMatrix());

		// ゲームの処理
		// 三角形
		model->Update();

		//sprite->Update();

		// ImGui
		ImGui::ColorEdit4("Color", &model->GetColor().color.x);
		ImGui::SliderFloat3("Scale", &model->GetTransform().scale.x, 0.0f, 5.0f);
		ImGui::SliderFloat3("Rotate", &model->GetTransform().rotate.x, 0.0f, 5.0f);
		ImGui::SliderFloat3("Translate", &model->GetTransform().translate.x, -5.0f, 5.0f);
		ImGui::Checkbox("useMonsterBall", &model->GetUseMonsterBallRef());
		ImGui::Text("useMonsterBall_: %s", model->GetUseMonsterBallRef() ? "true" : "false"); // 変更確認用
		ImGui::DragFloat("intensity", &model->GetIntensity(), 0.01f);
		ImGui::DragFloat3("lightDirection", &model->GetLightDirection().x, 0.01f);
		ImGui::DragFloat3("fogCenter", &model->GetFogParam().fogCenter.x, 0.01f);
		ImGui::DragFloat3("fogColor", &model->GetFogParam().fogColor.x, 0.01f);
		ImGui::SliderFloat("radius", &model->GetFogParam().radius, 0.0f, 50.0f);
		ImGui::SliderFloat("fogIntensity", &model->GetFogParam().fogIntensity, 0.0f, 1.0f);
		//ImGui::SliderFloat3("spriteTranslate", &sprite->GetTransform().translate.x, -5.0f, 5.0f);
		//ImGui::DragFloat2("UVTranslate", &sprite->GetUVTransform().translate.x, 0.01f, -10.0f, 10.0f);
		//ImGui::DragFloat2("UVScale", &sprite->GetUVTransform().scale.x, 0.01f, -10.0f, 10.0f);
		//ImGui::SliderAngle("UVRotate", &sprite->GetUVTransform().rotate.z);

		// 描画開始
		dxCommon->BeginFrame();

		model->SetPipelineState(dxCommon->GetGraphicsPipelineState());
		model->SetRootSignature(dxCommon->GetRootSignature());
		model->SetTextureHandle(textureManager_->GetTextureSrvHandleGPU());
		model->SetTextureHandle2(textureManager_->GetTextureSrvHandleGPU2());
		model->SetTextureHandle3(textureManager_->GetTextureSrvHandleGPU3());

		// 3Dモデル描画
		model->Draw(dxCommon->GetCommandList());

		// Sprite描画
		//sprite->Draw(dxCommon->GetCommandList());

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