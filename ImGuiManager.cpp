#include "ImGuiManager.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "WinApp.h"

using namespace Microsoft::WRL;

void ImGuiManager::SetModel(const std::shared_ptr<Model>& model) {
	model_ = model; // 同じインスタンスを共有 {
}

void ImGuiManager::Initialize(HWND hwnd, ComPtr<ID3D12Device> device, UINT bufferCount, DXGI_FORMAT format, ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap) {
	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device.Get(), bufferCount, format, srvDescriptorHeap.Get(), srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	model_ = std::make_shared<Model>();
}

void ImGuiManager::BeginFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::UpdateUI(Material& material, Transform& transform) {
	ImGui::ShowDemoWindow();
	ImGui::SliderFloat3("Color", &material.color.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Translate X", &transform.translate.x, 0.0f, 640);
	ImGui::SliderFloat("Translate Y", &transform.translate.y, 0.0f, 360);
	ImGui::Checkbox("useMonsterBall", &model_->GetUseMonsterBallRef());
	ImGui::Text("useMonsterBall_: %s", model_->GetUseMonsterBallRef() ? "true" : "false"); // 変更確認用
}

void ImGuiManager::Render(ComPtr<ID3D12GraphicsCommandList> commandList) {
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
}

void ImGuiManager::Finalize() {
	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
