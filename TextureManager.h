#pragma once
#include "externals/DirectXTex/DirectXTex.h"
#include "MaterialData.h"
#include <d3d12.h>
#include <string>
#include <vector>
#include <wrl.h>

class TextureManager {
public:
	~TextureManager();
	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, MaterialData& materialData);
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU2() { return textureSrvHandleGPU2_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU3() { return textureSrvHandleGPU3_; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);
	void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
	DirectX::ScratchImage mipImage_;
	DirectX::ScratchImage mipImage2_;
	DirectX::ScratchImage mipImage3_;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2_;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource3_;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2_;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU3_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU3_;
};
