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
}

void ImGuiManager::BeginFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
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
