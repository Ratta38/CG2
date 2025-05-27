#pragma once
#include "Material.h"
#include "Model.h"
#include "Transform.h"
#include "Vector4.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include <wrl.h>

class ImGuiManager {
public:
	void Initialize(HWND hwnd, Microsoft::WRL::ComPtr<ID3D12Device> device, UINT bufferCount, DXGI_FORMAT format, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap);
	void BeginFrame();
	void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);
	void Finalize();
	void SetModel(const std::shared_ptr<Model>& model);

private:
	std::shared_ptr<Model> model_;
};
