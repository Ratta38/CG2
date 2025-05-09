#pragma once
#include "Vector4.h"
#include "Transform.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class ImGuiManager {
public:
	void Initialize(HWND hwnd, Microsoft::WRL::ComPtr<ID3D12Device> device, UINT bufferCount, DXGI_FORMAT format, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap);
	void BeginFrame();
	void UpdateUI(Vector4& color, Transform& transform);
	void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);
	void Finalize();
};
